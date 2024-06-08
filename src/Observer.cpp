#ifndef OBSERVER
#define OBSERVER

class Observer {
public:
    virtual void notifyPressed() = 0;
    virtual void notifyReleased() = 0;
    virtual ~Observer() = default;
};

#endif /* OBSERVER */
