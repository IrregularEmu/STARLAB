#pragma once
#include "flight.hpp"
// Example host fixture, not a UI or a calibrated vehicle. Stations measured from nose.
struct FixtureHost : flight::geometry::Host {
 flight::geometry::Design design; double g=flight::gravity,exampleThrust=120;
 FixtureHost(){
  using namespace flight::geometry;
  design.diameter=.16;design.baseOwner="boattail";
  design.parts={{"cone",Kind::Cone,0,.36,0,.07},{"tube",Kind::Tube,.36,.55,.07,.07},
    {"transition",Kind::Transition,.91,.12,.07,.08},{"boattail",Kind::Boattail,1.03,.12,.08,.05},
    {"fins",Kind::Fins,.92,.16,.08,0,4,.09,.16,.07,.06,.002}};
 }
 const flight::geometry::Design& activeDesign(const flight::geometry::Request&)const override{return design;}
 flight::geometry::Conditions conditions(const flight::geometry::Request& r)const override{
  using namespace flight;auto air=atmosphere(980-r.position.z,false);
  geometry::Conditions c{};c.rho=air.rho;c.soundSpeed=air.a;c.mu=air.mu;c.temperature=air.T;c.pressure=air.P;
  c.mass=10;c.cgStation=.70;c.groundVelocityB=r.velocityB;c.windB=inverseRotate(r.orientation,{1,0,0});
  c.gravityB=inverseRotate(r.orientation,{0,0,g});c.firing=r.firing;c.motorExitArea=.001;
  c.thrustB={r.firing?exampleThrust:0.,0,0};return c;
 }
};
inline flight::Config fixtureScenario(const FixtureHost& host){
 flight::Config c{};c.host=&host;c.dt=.02;c.site=980;c.startUp=15;c.speed=50;c.theta=70*flight::pi/180;
 flight::Phase p{};p.name="example";p.duration=.35;p.burn=.15;p.cg=-.70;p.m0=p.m1=10;p.I0=p.I1={.1,2,2};
 c.phases.push_back(p);return c;
}

