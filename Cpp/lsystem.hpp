#include <vmath.hpp>
#include <pugixml.hpp>
#include <tinythread.hpp>
#include <list>

class lsystem
{
public:

    /// Parse given XML file, evaluate rules, populate member curve
    void Evaluate(const char* filename, unsigned int seed = 0);
    
    struct CurvePoint {
        vmath::Point3 P;
        vmath::Vector3 N;
    };
    
    typedef std::list<CurvePoint*> Curve;

    /// The curve generated by the L-system
    const Curve& GetCurve() const { return _curve; }

    lsystem(bool mt = true) : _isThreading(mt) {}
    ~lsystem();
    
private:

    struct StackEntry {
        pugi::xml_node Node;
        int Depth;
        vmath::Matrix4 Transform;
    };

    typedef std::list<lsystem::StackEntry> Stack;

    struct ThreadInfo {
        StackEntry Entry;
        Curve Result;
        lsystem* Self;
        unsigned int Seed;
        unsigned int StackOffset;
    };
    
    friend void _ProcessRule(void* arg);
    void _ProcessRule(const StackEntry& entry, Curve* result, unsigned int seed, unsigned int stackOffset);

    pugi::xml_document _doc;
    Curve _curve;
    int _curveLength;
    int _maxDepth;
    int _maxThreads;
    const bool _isThreading;
    
    // TODO combine into a semaphore primitive?
    int _threadsComplete;
    int _threadsCompute;
    tthread::mutex _mutexComplete;
    tthread::mutex _mutexCompute;
    tthread::condition_variable _condComplete;
    tthread::condition_variable _condCompute;
};

