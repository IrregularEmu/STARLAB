#include "geometry_example.hpp"
using namespace flight;
int main(int argc,char** argv){
 try{
  require(argc==5,"Usage: ucus scenario.txt 3|6 history.csv surfaces.csv");
  std::string mode=argv[2];require(mode=="3"||mode=="6","Use 3 or 6");
  require(std::string(argv[3])!=argv[4]&&std::string(argv[1])!=argv[3]&&std::string(argv[1])!=argv[4],"Use distinct file paths");
  FixtureHost example;bool envelope=std::string(argv[1])=="--mach-flight";
  auto c=(envelope||std::string(argv[1])=="--geometry")?fixtureScenario(example):readConfig(argv[1]);
  if(envelope){c.speed=220;c.theta=pi/2;c.phases[0].duration=4;c.phases[0].burn=1.2;example.exampleThrust=5200;}
  if(c.host){
   auto state=initial(c);geometry::Request r{0,0,c.phases[0].name,state.pos,state.attitude,state.vel,state.omega,true};
   auto result=geometry::evaluateAt(example,r);
   std::cout<<"initial component,CP_station_m,CD,CL,Cm_CG\n";
   for(auto& p:result.parts){std::cout<<p.id<<',';if(p.cpDefined)std::cout<<p.cp;else std::cout<<"undefined";std::cout<<','<<p.cd<<','<<p.cl<<','<<p.cm<<'\n';}
  }
  std::ofstream h(argv[3]),s(argv[4]);require(h&&s,"Cannot open output");
  simulate(c,mode=="3",&h,&s);require(h.good()&&s.good(),"Write failed");
  return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
