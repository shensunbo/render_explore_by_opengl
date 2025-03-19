#include "configParser/ConfigParser.h"

int main(){
    CarModelConfigParser parser;
    parser.loadConfigFile("./res/model/ford/vehicle_info.json");
    return 0;
}