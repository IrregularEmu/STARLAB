#include "geometry_example.hpp"
using namespace flight;
void near(double a,double b,double tol,const char* msg){require(std::abs(a-b)<=tol,msg);}
int main(int argc,char**argv){try{
 require(argc==2,"tests scenario.txt");auto c=readConfig(argv[1]);
 auto air=atmosphere(0,false);near(air.rho,1.225,0.0001,"sea level");
 near(atmosphere(11000*6356766./(6356766.-11000),false).P,22632,2,"layer boundary");
 near(atmosphere(0,true).P,101290*std::pow((15.04+273.1)/288.08,5.256),.01,"archive atmosphere");
 auto p=c.phases[0];State s=initial(c);s.attitude={1,{}};s.vel={10,0,0};p.surfaces.resize(1);
 auto& sf=p.surfaces[0];sf.ref={p.cg-1,0,0};sf.area=.01;sf.d=.2;
 for(auto& row:sf.table){row.c={};row.c[CDoff]=.5;row.c[CLa]=2;}
 auto a=evaluate(c,p,s,0,false,true);near(a.total.f.x,-a.Q*.005,1e-12,"drag");
 s.vel={10,0,1};a=evaluate(c,p,s,0,false,true);near(a.total.m.y,a.total.f.z,1e-12,"CG transfer");
 near(dot(a.total.f,s.vel),-a.Q*.005*a.V,1e-12,"drag power");
 s.vel={0,0,0};a=evaluate(c,p,s,0,false,true);near(length(a.total.f)+length(a.total.m),0,0,"zero airspeed");
 s.vel={10,0,0};for(auto& row:sf.table){row.c[Cma]=0;row.c[Cl0]=.1;}a=evaluate(c,p,s,0,false,false);
 near(a.total.m.x,a.Q*.01*.2*.1,1e-12,"moment length");bool rejected=false;
 try{evaluate(c,p,s,0,false,true);}catch(const std::runtime_error&){rejected=true;}require(rejected,"3DOF accepted roll");
 Config w=c;w.wind={5,2,0};s.vel={15,2,0};auto b=evaluate(w,p,s,0,false,false);
 near(length(b.total.f-a.total.f),0,1e-12,"relative wind invariance");
 auto A=simulate(c,true,nullptr,nullptr),B=simulate(c,false,nullptr,nullptr);near(length(A.pos-B.pos),0,1e-11,"planar equivalence");
 // Independent exact ballistic solution across a shortened single coast phase.
 Config coast=c;coast.phases.resize(1);auto& ph=coast.phases[0];ph.duration=1;ph.burn=ph.thrust=0;ph.m1=ph.m0;ph.I1=ph.I0;
 for(auto& f:ph.surfaces)for(auto& row:f.table)row.c={};
 auto res=simulate(coast,true,nullptr,nullptr);double vx=c.speed*std::cos(c.theta),vz=-c.speed*std::sin(c.theta);
 near(res.pos.x,vx,1e-9,"ballistic north");near(res.pos.z,-c.startUp+vz+.5*gravity,1e-9,"ballistic down");
 Config fine=c;fine.dt*=.5;auto F=simulate(fine,false,nullptr,nullptr);
 double diff=length(A.pos-F.pos);require(diff<.001,"Step convergence >1 mm");
 // Nonplanar yaw moment response must be finite, visible and correctly signed.
 for(auto& row:p.surfaces[0].table)row.c[Cnb]=1;
 s.vel={20,1,0};s.omega={};a=evaluate(c,p,s,0,false,false);require(a.rate.omega.z>0,"yaw sign");
 p.motor={{0,50},{0.5,100},{p.burn,0}};near(sampleCurve(p.motor,.25,&MotorPoint::force),75,1e-12,"motor interpolation");
 Config lateral=c;lateral.wind.y=2;auto lat=simulate(lateral,false,nullptr,nullptr);
 require(std::abs(lat.pos.y)>1e-4,"Missing crosswind drift");
 // Independent moment-only derivative at a known alpha and pitch inertia.
 p.I0=p.I1={1,4,4};p.surfaces[0].ref={p.cg,0,0};s.vel={20,0,2};s.omega={};
 a=evaluate(c,p,s,0,false,false);
 for(auto& row:p.surfaces[0].table){row.c={};row.c[Cma]=2/(a.Q*sf.area*sf.d*a.alpha);}
 near(evaluate(c,p,s,0,false,true).rate.omega.y,.5,1e-12,"pitch acceleration");
 // Constant mass, zero aero, fixed orientation. Exact thrust pulse followed by coast.
 ph.burn=.37;ph.thrust=ph.m0*12;coast.dt=.11;
 for(bool planar:{true,false}){
  auto end=simulate(coast,planar,nullptr,nullptr);
  double shift=12*ph.burn*(1-ph.burn/2);
  near(end.pos.x,vx+shift*std::cos(coast.theta),1e-9,"powered horizontal motion");
  near(end.pos.z,-coast.startUp+vz+.5*gravity-shift*std::sin(coast.theta),1e-9,"powered vertical motion");
 }
 auto probe=initial(coast);auto active=evaluate(coast,ph,probe,0,true,false);
 auto off=evaluate(coast,ph,probe,ph.burn,false,false);
 near(length(off.forces.thrust),0,0,"burn cutoff");
 near(active.air.P,off.air.P,0,"motor must not affect atmosphere");
 for(int fault=0;fault<2;++fault){
  auto bad=active;
  if(fault==0)bad.forces.net=bad.forces.net+bad.forces.weight; // count gravity twice
  else bad.rate.vel=bad.rate.vel+(1/bad.mass)*bad.forces.thrust; // count thrust twice
  bool caught=false;try{auditForces(bad,probe,unit(probe.attitude),ph,0);}
  catch(const std::runtime_error& e){std::cout<<"Injected "<<fault<<": "<<e.what()<<"\n";caught=std::string(e.what()).find("Force balance")!=std::string::npos;}
  require(caught,"Injected force error escaped detection "+std::to_string(fault));
 }
 std::cout<<"PASS: exact thrust pulse (3/6DOF), motor cutoff, atmosphere isolation, double gravity and double thrust detected.\n";
 std::cout<<"PASS: atmosphere, loads, CG, wind, plane, ballistic, yaw, motor, pitch, dt-halving. Endpoint difference (m): "<<diff<<"\n";
 FixtureHost host;auto inputs=fixtureScenario(host);auto st=initial(inputs);
 geometry::Request req{0,0,"example",st.pos,st.attitude,{40,0,2},st.omega,false};
 auto cond=host.conditions(req);auto geom=geometry::evaluate(host.design,cond);
 near(geom.parts[0].cp,.24,1e-12,"cone CP at 2L/3");
 require(!geom.parts[1].cpDefined&&geom.parts[1].cd>0,"tube CP/drag");
 require(geom.parts[2].cnSlope>0&&geom.parts[3].cnSlope<0,"transition versus boattail sign");
 require(geom.parts[4].cnSlope>0&&geom.parts[4].cm<0,"fin restoring moment");
 double cd=0;for(auto& part:geom.parts)cd+=part.cd;near(cd,geom.cd,1e-14,"sum component CD");
 auto shifted=cond;shifted.cgStation+=.1;
 auto newGeom=geometry::evaluate(host.design,shifted);
 near(length(newGeom.momentCG-geom.momentCG-cross({.1,0,0},geom.aerodynamic)),0,1e-10,"CG shift sign");
 shifted=cond;shifted.firing=true;
 require(geometry::evaluate(host.design,shifted).cd<geom.cd,"base drag with motor on");
 auto end3=simulate(inputs,true,nullptr,nullptr),end6=simulate(inputs,false,nullptr,nullptr);
 near(length(end3.pos-end6.pos),0,1e-10,"host integrated 3/6");
 host.g=8.5;auto lowerGravity=simulate(inputs,true,nullptr,nullptr);
 require(lowerGravity.pos.z<end3.pos.z,"external gravity ignored");
 std::cout<<"PASS: geometry CP/CD/CL/Cm, motor base area, external gravity and host integration.\n";
 // Fixed nonzero incidence: isolate Mach dependence from the trajectory.
 auto machSample=[&](double m){
  auto v=cond;double angle=3*pi/180;
  v.groundVelocityB=v.windB+(m*v.soundSpeed)*Vec{std::cos(angle),0,std::sin(angle)};
  return geometry::evaluate(host.design,v);
 };
 for(double edge:{.5,.8,1.,1.3,2.}){
  auto l=machSample(edge-1e-7),r=machSample(edge+1e-7);
  for(size_t i=0;i<l.parts.size();++i){
   near(l.parts[i].cd,r.parts[i].cd,1e-5,"CD boundary jump");
   near(l.parts[i].cl,r.parts[i].cl,1e-5,"CL boundary jump");
   near(l.parts[i].cm,r.parts[i].cm,1e-5,"Cm boundary jump");
   near(l.parts[i].cp,r.parts[i].cp,1e-5,"CP boundary jump");
  }
  near(l.cp,r.cp,1e-5,"vehicle CP boundary jump");
 }
 near(geometry::baseDrag(2),.125,1e-14,"Mach 2 base");
 near(geometry::prandtlMeyer(2)*180/pi,26.3797608,1e-7,"PM angle at Mach 2");
 double turn=geometry::prandtlMeyer(3)-geometry::prandtlMeyer(2);
 near(geometry::expansionDeficit(2,turn),(1-std::pow(1.8/2.8,3.5))/2.8,1e-12,"Expansion pressure ratio");
 near(geometry::conePressure(2,.2),.084+.1/std::sqrt(3.),1e-13,"Mach 2 cone");
 near(geometry::conePressure(1,.2),.2,1e-13,"sonic anchor");
 require(machSample(.75).parts.back().cnSlope>machSample(.15).parts.back().cnSlope,"subsonic fin trend");
 require(machSample(2).parts.back().cnSlope>machSample(2.8).parts.back().cnSlope,"supersonic fin trend");
 bool exceeded=false;try{machSample(3.01);}catch(const std::runtime_error&){exceeded=true;}require(exceeded,"Mach envelope");
 for(int j=0;j<=300;++j){auto v=machSample(j*.00999);require(std::isfinite(v.cp+v.cl+v.cd+v.cm),"Mach sweep");}
 std::cout<<"PASS: fixed-incidence Mach sweep, boundary continuity, sonic anchor and supersonic references.\n";
 auto inlet=cond;inlet.pressure=inlet.rho*inlet.soundSpeed*inlet.soundSpeed/1.4; // rho*a^2/gamma
 inlet.groundVelocityB=inlet.windB+Vec{2*inlet.soundSpeed,0,0};
 auto tail=geometry::evaluate(host.design,inlet).parts[3];
 double expectedTurn=std::atan(.01/.12)+std::atan(.03/.12);
 near(tail.turnRad,expectedTurn,1e-12,"Transition exit tangent");
 require(tail.inletMethod=="freestream_proxy","Missing fallback flag");
 inlet.tailInlets={{"boattail",1.8,1.15*inlet.pressure}};
 auto local=geometry::evaluate(host.design,inlet).parts[3];
 double qInf=.5*inlet.rho*4*inlet.soundSpeed*inlet.soundSpeed;
 double p1=inlet.tailInlets[0].pressurePa;
 double wall=p1*(1-.7*1.8*1.8*geometry::expansionDeficit(1.8,expectedTurn));
 double change=((inlet.pressure-wall)/qInf-geometry::expansionDeficit(2,expectedTurn))*(.08*.08-.05*.05)/(.08*.08);
 near(local.cd-tail.cd,change,1e-11,"Local pressure force normalization");
 require(local.inletMethod=="local_supersonic","Local inlet flag");
 auto cylinder=host.design;cylinder.parts[2].kind=geometry::Kind::Tube;cylinder.parts[2].foreRadius=.08;
 near(geometry::evaluate(cylinder,inlet).parts[3].turnRad,std::atan(.03/.12),1e-12,"Tube exit tangent");
 std::cout<<"PASS: transition/tube turn, proxy flag and local inlet pressure force.\n";
 return 0;
 }catch(const std::exception&e){std::cerr<<e.what()<<"\n";return 1;}}
