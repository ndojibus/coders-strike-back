#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{
    const int angleForMaxThrust = 90;
    const int distForTheBoost = 5000;
    const int maxThrust = 100;
    int thrust = 0;
    bool boost = false;


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

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;


        // You have to output the target position
        // followed by the power (0 <= thrust <= 100)
        // i.e.: "x y thrust"

        thrust = (nextCheckpointAngle > angleForMaxThrust  || nextCheckpointAngle < -angleForMaxThrust) ? 10 : maxThrust;
        boost = (nextCheckpointDist>distForTheBoost && nextCheckpointAngle == 0);
        
        cout << nextCheckpointX << " " << nextCheckpointY << " " <<  ((boost) ? "BOOST" : std::to_string(thrust))<< endl;
    }
}