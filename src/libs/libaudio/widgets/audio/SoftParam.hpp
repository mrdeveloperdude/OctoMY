#ifndef SOFTPARAM_H
#define SOFTPARAM_H



template <typename T>
struct SoftParam{
    T val=0;
    T target=0;
    T damp=0.9;
    T idamp=1.0-damp;
public:
    inline void setDamp(T d){
        damp=d;
        idamp=1.0-damp;
    }
    inline void setTarget(T t){
        target=t;
    }
    inline T value(){
        val=val*damp+target*idamp;
        return val;
    }
};



#endif // SOFTPARAM_H
