#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

#define PI 3.14159265

using namespace std;

class PidController
{
public:
    PidController(double kP, double kI, double kD) : 
        _kProportional(kP),
        _kIntegral(kI),
        _kDerivative(kD)
    {};

    int getThurst(int distance, int angle);
    void reset();


private:
    // control constants (ignoring _dt as queals to 1) 
    double _kProportional = 0;
    double _kIntegral = 0;
    double _kDerivative = 0;

    int _pre_angle = 0; // previous registered angle
    double _pre_error = 0; // previous registered error
    double _integralSum = 0; // accumulate sum of errors
};

void PidController::reset()
{
    _integralSum = 0;

    // These two might not need to be reset, to be tested
    _pre_angle = 0;
    _pre_error = 0;
}

int PidController::getThurst(int distance, int angle)
{
    // angular change (used to assess how sharp is the current sterrting)
    const int angleDiff = abs(angle - _pre_angle);

    // error used is just the normalized distance
    const double error = distance / 100;
    cerr << "error: " << error << endl;

    // P = how far is the vehicle
    const double proportional = _kProportional * error;
    
    // I = increasies the longer the vehicle is far from the set point
    _integralSum += error;
    const double integral = _kIntegral *_integralSum;

    // D = current speed
    const double derivative = _kDerivative * (error - _pre_error);

    int output = proportional + derivative;
    // OPTIMIZATION ATTEMPT TO BE IMPROVED 
    // only use the integral part if the direction is under a given angle
    // used to avoid some circular moves in case of collisions
    if(abs(angle) < 45)
        output += integral;

    // If the vehichle is not doing big curves (angelDiff)
    // And has to a closer direction to the target (angle)
    // Go full speed
    if(abs(angleDiff) < 2 ||  abs(angle) <= 15)
    {
        output = 100;
    }

    cerr << "output: " << output << endl;
    
    // Normaliza the thurst
    if(output > 100)
        output = 100;
    else if( output < 0)
        output = 0;

    // update previou error and angle
    _pre_error = error;
    _pre_angle = angle;
    return output;
}

class Vec2D{

    public:
        Vec2D(double x0, double y0) : x(x0), y(y0) {}
        double getX() const {return x;}
        double getY() const {return y;}
        bool operator==(const Vec2D & v2) const{
            return(v2.getX() == x && v2.getY() == y);
        }
        bool operator!=(const Vec2D & v2) const{
            return (v2.getX() != x || v2.getY() != y);
        }
        double magnitude(){
            return sqrt(x*x + y*y);
        }
        void normalize()
        {
            if(magnitude()!=0)
            {
                x /= magnitude();
                y /= magnitude();
            }
        }
        friend std::ostream & operator<<(ostream& os, const Vec2D & v);

    private:
        double x, y;
};

std::ostream & operator<<(ostream& os, const Vec2D & v){
    os<< "Vec2D: x -> " << v.getX() <<" y-> " << v.getY() << endl;
    return os;
};
class CheckPointManager{
    
    public:
        bool addCheckpoint(Vec2D p){
            if(prevCheckpoint != p)
            {
                bool found =( std::find(checkpoints.begin(), checkpoints.end(), p) != checkpoints.end());
                if(!found){
                    checkpoints.push_back(p);
                }
                allCheckpointsSaved = found ;
                prevCheckpoint =p;
                return !found;
            }
            return false;
        }
        Vec2D getNextCheckpoint(Vec2D p )
        {
            std::list<Vec2D>::iterator it;
            for (it=checkpoints.begin(); it != checkpoints.end(); ++it)
            {
                if (*it == p) break;
            }
            return (++it != checkpoints.end()) ? *it : checkpoints.front();
        }
        std::list<Vec2D> getCheckpoints() const {
            return checkpoints;
        }
        bool isFirstLapFinished() {return allCheckpointsSaved;};
        friend std::ostream & operator<<(ostream& os, const CheckPointManager & pt);

    private:
        std::list<Vec2D> checkpoints;
        bool allCheckpointsSaved = false;
        Vec2D prevCheckpoint{0,0};
};

std::ostream & operator<<(ostream& os, const CheckPointManager & pt){

    for (std::list<Vec2D>::iterator it=pt.getCheckpoints().begin(); it != pt.getCheckpoints().end(); ++it)
        os<< pt;
    return os;
}

int main()
{
    const int angleForMaxThrust = 90;
    const int distForTheBoost = 5000;
    const int distMinToCheckpoint = 1500;
    const int maxThrust = 100;
    const int minThrust = 10;
   
    int thrust = maxThrust;
    bool boost = false;

    CheckPointManager checkManager;
    Vec2D direction{0,0};
    Vec2D prev_dir{0,0};

    PidController control(2, 0.5, 0.2);

    // game loop
    while (1) {
        int x;
        int y;
        int nextCheckpointX; // x position of the next check point
        int nextCheckpointY; // y position of the next check point
        int nextCheckpointDist; // distance to the next checkpoint
        int nextCheckpointAngle; // angle between your pod orientation and the direction of the next checkpoint
        cin >> x >> y >> nextCheckpointX >> nextCheckpointY >> nextCheckpointDist >> nextCheckpointAngle; cin.ignore();
        int opponentX;
        int opponentY;
        cin >> opponentX >> opponentY; cin.ignore();

        direction = Vec2D(nextCheckpointX,nextCheckpointY);

        if(!checkManager.isFirstLapFinished())
        {
            checkManager.addCheckpoint(direction);
        }

        if(checkManager.isFirstLapFinished() && nextCheckpointDist < distMinToCheckpoint)
        {
            direction = checkManager.getNextCheckpoint(direction);
        }

        if(direction != prev_dir)
            control.reset();

        //calculating des vel
        Vec2D des_vel{direction.getX() - x, direction.getY() - y};
        des_vel.normalize();

        //calculating current dir
        float radian = (-nextCheckpointAngle) * PI / 180;
        Vec2D vel{des_vel.getX() * cos(radian) - des_vel.getY() * sin(radian), des_vel.getY() * cos(radian) + des_vel.getX() * sin(radian)};
        vel.normalize();

        //steering dir
        Vec2D sterring = {des_vel.getX() - vel.getX(),des_vel.getY() - vel.getY() };
        sterring.normalize();

        Vec2D finalDirection{direction.getX() + sterring.getX() * 100, direction.getY() + sterring.getY() * 100};

        thrust = control.getThurst(nextCheckpointDist, nextCheckpointAngle);

        cout << (int)finalDirection.getX() << " " << (int)finalDirection.getY() << " " <<  int(thrust) << endl;
    }
}