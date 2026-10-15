#include<iostream>
#include<string>
#include"Gate.hpp"
using namespace std;

Signal::Signal(bool initialValue):bitValue(initialValue){}

bool Signal::getValue() const{
    return bitValue;
}

Signal Signal::operator&(const Signal& rhs) const{
    return Signal(bitValue&&rhs.bitValue);
}

Signal Signal::operator|(const Signal& rhs) const{
    return Signal(bitValue||rhs.bitValue);
}

Signal Signal::operator^(const Signal& rhs) const{
    return Signal(bitValue!=rhs.bitValue);
}

Signal Signal::operator~() const{
    return Signal(!bitValue);
}

bool Signal::operator==(const Signal& rhs) const{
    return bitValue==rhs.bitValue;
}

LogicGate::LogicGate(string name,int count,bool first,bool second){
    gateName=name;
    inputCount=count;
    outputValue=false;
    firstInput=first;
    secondInput=second;
    evaluate();
}

LogicGate::~LogicGate(){}

bool LogicGate::evaluate(){
    if(gateName=="AND" && inputCount==2){
        outputValue=firstInput && secondInput;
    }
    else if(gateName=="OR" && inputCount==2){
        outputValue=firstInput || secondInput;
    }
    else if(gateName=="NOT" && inputCount==1){
        outputValue=!firstInput;
    }
    else if(gateName=="XOR" && inputCount==2){
        outputValue=firstInput!=secondInput;
    }
    else if(gateName=="NAND" && inputCount==2){
        outputValue=!(firstInput && secondInput);
    }
    else if(gateName=="NOR" && inputCount==2){
        outputValue=!(firstInput || secondInput);
    }

    return outputValue;
}

bool LogicGate::getOutput() const{
    return outputValue;
}

ostream& operator<<(ostream& screen,const LogicGate& item){
    screen<<item.gateName<<" Gate: ";

    if(item.inputCount==1){
        screen<<"Input = "<<item.firstInput;
    }
    else{
        screen<<"Inputs = ("<<item.firstInput<<","<<item.secondInput<<")";
    }

    screen<<", Output = "<<item.outputValue;
    return screen;
}
#ifdef STANDALONE_TEST
int main(){
    cout<<"CircuitLab-C++ \n";
    cout<<"Logic Gates and Operator Overloading\n\n";

    Signal firstBit(1),secondBit(0);

    cout<<"Operator Overloading Tests:\n";
    cout<<"1 & 0 = "<<(firstBit&secondBit).getValue()<<'\n';
    cout<<"1 | 0 = "<<(firstBit|secondBit).getValue()<<'\n';
    cout<<"1 ^ 0 = "<<(firstBit^secondBit).getValue()<<'\n';
    cout<<"~1 = "<<(~Signal(1)).getValue()<<'\n';

    cout<<"1 == 1: "<<(Signal(1)==Signal(1))<<'\n';
    cout<<"1 == 0: "<<(Signal(1)==Signal(0))<<'\n';

    cout<<"\nLogic Gate Tests:\n";

    LogicGate andGate("AND",2,1,1);
    LogicGate orGate("OR",2,1,0);
    LogicGate notGate("NOT",1,1);
    LogicGate xorGate("XOR",2,1,0);
    LogicGate nandGate("NAND",2,1,1);
    LogicGate norGate("NOR",2,0,0);

    cout<<andGate<<'\n';
    cout<<orGate<<'\n';
    cout<<notGate<<'\n';
    cout<<xorGate<<'\n';
    cout<<nandGate<<'\n';
    cout<<norGate<<'\n';

    cout<<"\nOutput Verification:\n";
    cout<<"AND output == 1: "<<(andGate.getOutput()==1)<<'\n';
    cout<<"NOT output == 0: "<<(notGate.getOutput()==0)<<'\n';

    return 0;
}
#endif