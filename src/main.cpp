#include "Application/Application.hpp"
#include "Utils/Config.hpp"
#include <iostream>
#include <string>

int main(int argc,char**argv){std::string path=std::string(GESTURE3D_CONFIG_DIR)+"/config.json";g3d::AppConfig config=g3d::loadConfig(path);if(argc>1&&std::string(argv[1])=="--replay"&&argc>2)config.replayFile=argv[2];else if(argc>1)config=g3d::loadConfig(argv[1]);g3d::Application app;if(!app.initialize(config)){std::cerr<<"Gesture3D failed to initialize. See the log for details.\n";app.shutdown();return 1;}const int result=app.run();app.shutdown();return result;}
