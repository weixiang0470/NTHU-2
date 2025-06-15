#include "systemc.h"
#include "../LeNet.h"
#include "../Monitor.h"
#include "../NI.h"
#include "../Processing_Element.h"
#include "../RAM.h"
#include "../ROM.h"
#include "../Router.h"
#include "../Tile.h"
#include "../clockreset.h"
#include "cassert"
#include "cwr_dynamic_loader.h"
#include "cwr_sc_dynamic_stubs.h"
#include "cwr_sc_hierarch_module.h"
#include "cwr_sc_object_creator.h"
#include "scmlinc/scml_abstraction_level_switch.h"
#include "scmlinc/scml_property_registry.h"

using namespace conf;
using namespace std;


class Clock0Creator : public ScObjectCreatorBase
{
public:
  static unsigned int creationVerboseMode() {
    const char * const env_var_val = ::getenv("SNPS_SLS_DYNAMIC_CREATION_VERBOSE");
    return env_var_val ? (::atoi(env_var_val)) : 3;
  }
  sc_object* create ( const string& name ) {
    string hierach_name = getHierarchicalName(name);
    int division = scml_property_registry::inst().getIntProperty(
	      scml_property_registry::CONSTRUCTOR, hierach_name, "division");

    int cycle = scml_property_registry::inst().getIntProperty(
	      scml_property_registry::CONSTRUCTOR, hierach_name, "cycle");

    if (scml_property_registry::inst().hasProperty(scml_property_registry::MODULE, scml_property_registry::BOOL, hierach_name, "runtime_disabled") && 
        scml_property_registry::inst().getBoolProperty(scml_property_registry::MODULE, hierach_name, "runtime_disabled")) {
      sc_module_name n(name.c_str());
      if (creationVerboseMode() >= 6) { std::cout << "Clock/Clock: STUB for " << hierach_name << " created." << std::endl; }
      conf::stub *result = new conf::stub(n);
       conf::stub_port_registrator<>::register_stub_port(&Clock::clk, "clk" , string(static_cast<sc_object*>(result)->name()) + ".clk" );
      return result;
    } else {
      if (creationVerboseMode() >= 6) { std::cout << "Clock/Clock: " << hierach_name << " created." << std::endl; }
      Clock* result = new Clock(name.c_str(), division, cycle);
       cwr_sc_object_registry::inst().addPort(&result->clk, string(static_cast<sc_object*>(result)->name()) + ".clk" );
      return result;
    }
  }
};

template <class BC>
class BusCreator : public ScObjectCreatorBase
{
  sc_object* create (const string& name) {
    ScGeneratedBaseBlockCreatorHelper helper(getHierarchicalName(name));
    if (helper.is_runtime_disabled()) {
      return new conf::StubBusCreator(name.c_str(), helper);
    } else {
      return new BC(name.c_str(), helper);
    }
  }
};

template <class GBC>
class GeneratedBlockCreator : public ScObjectCreatorBase
{
  sc_object* create (const string& name) {
    ScGeneratedBaseBlockCreatorHelper helper(getHierarchicalName(name));
    if (helper.is_runtime_disabled()) {
      return new conf::StubBusCreator(name.c_str(), helper);
    } else {
      return new GBC(name.c_str(), helper);
    }
  }
};



class LeNet0Creator : public ScObjectCreatorBase
{
public:
  static unsigned int creationVerboseMode() {
    const char * const env_var_val = ::getenv("SNPS_SLS_DYNAMIC_CREATION_VERBOSE");
    return env_var_val ? (::atoi(env_var_val)) : 3;
  }
  sc_object* create ( const string& name ) {
    string hierach_name = getHierarchicalName(name);
    if (scml_property_registry::inst().hasProperty(scml_property_registry::MODULE, scml_property_registry::BOOL, hierach_name, "runtime_disabled") && 
        scml_property_registry::inst().getBoolProperty(scml_property_registry::MODULE, hierach_name, "runtime_disabled")) {
      sc_module_name n(name.c_str());
      if (creationVerboseMode() >= 6) { std::cout << "LeNet/LeNet: STUB for " << hierach_name << " created." << std::endl; }
      conf::stub *result = new conf::stub(n);
       conf::stub_port_registrator<>::register_stub_port(&LeNet::clk, "clk" , string(static_cast<sc_object*>(result)->name()) + ".clk" );
       conf::stub_port_registrator<>::register_stub_port(&LeNet::reset, "reset" , string(static_cast<sc_object*>(result)->name()) + ".reset" );
       conf::stub_port_registrator<>::register_stub_port(&LeNet::rom_rd, "rom_rd" , string(static_cast<sc_object*>(result)->name()) + ".rom_rd" );
       conf::stub_port_registrator<>::register_stub_port(&LeNet::rom_addr, "rom_addr" , string(static_cast<sc_object*>(result)->name()) + ".rom_addr" );
       conf::stub_port_registrator<>::register_stub_port(&LeNet::rom_data_out, "rom_data_out" , string(static_cast<sc_object*>(result)->name()) + ".rom_data_out" );
       conf::stub_port_registrator<>::register_stub_port(&LeNet::ram_wr, "ram_wr" , string(static_cast<sc_object*>(result)->name()) + ".ram_wr" );
       conf::stub_port_registrator<>::register_stub_port(&LeNet::ram_addr, "ram_addr" , string(static_cast<sc_object*>(result)->name()) + ".ram_addr" );
       conf::stub_port_registrator<>::register_stub_port(&LeNet::ram_data_out, "ram_data_out" , string(static_cast<sc_object*>(result)->name()) + ".ram_data_out" );
       conf::stub_port_registrator<>::register_stub_port(&LeNet::ram_data_in, "ram_data_in" , string(static_cast<sc_object*>(result)->name()) + ".ram_data_in" );
       conf::stub_port_registrator<>::register_stub_port(&LeNet::result, "result" , string(static_cast<sc_object*>(result)->name()) + ".result" );
       conf::stub_port_registrator<>::register_stub_port(&LeNet::output_valid, "output_valid" , string(static_cast<sc_object*>(result)->name()) + ".output_valid" );
      return result;
    } else {
      if (creationVerboseMode() >= 6) { std::cout << "LeNet/LeNet: " << hierach_name << " created." << std::endl; }
      LeNet* result = new LeNet(name.c_str());
       cwr_sc_object_registry::inst().addPort(&result->clk, string(static_cast<sc_object*>(result)->name()) + ".clk" );
       cwr_sc_object_registry::inst().addPort(&result->reset, string(static_cast<sc_object*>(result)->name()) + ".reset" );
       cwr_sc_object_registry::inst().addPort(&result->rom_rd, string(static_cast<sc_object*>(result)->name()) + ".rom_rd" );
       cwr_sc_object_registry::inst().addPort(&result->rom_addr, string(static_cast<sc_object*>(result)->name()) + ".rom_addr" );
       cwr_sc_object_registry::inst().addPort(&result->rom_data_out, string(static_cast<sc_object*>(result)->name()) + ".rom_data_out" );
       cwr_sc_object_registry::inst().addPort(&result->ram_wr, string(static_cast<sc_object*>(result)->name()) + ".ram_wr" );
       cwr_sc_object_registry::inst().addPort(&result->ram_addr, string(static_cast<sc_object*>(result)->name()) + ".ram_addr" );
       cwr_sc_object_registry::inst().addPort(&result->ram_data_out, string(static_cast<sc_object*>(result)->name()) + ".ram_data_out" );
       cwr_sc_object_registry::inst().addPort(&result->ram_data_in, string(static_cast<sc_object*>(result)->name()) + ".ram_data_in" );
       cwr_sc_object_registry::inst().addPort(&result->result, string(static_cast<sc_object*>(result)->name()) + ".result" );
       cwr_sc_object_registry::inst().addPort(&result->output_valid, string(static_cast<sc_object*>(result)->name()) + ".output_valid" );
      return result;
    }
  }
};


class Monitor0Creator : public ScObjectCreatorBase
{
public:
  static unsigned int creationVerboseMode() {
    const char * const env_var_val = ::getenv("SNPS_SLS_DYNAMIC_CREATION_VERBOSE");
    return env_var_val ? (::atoi(env_var_val)) : 3;
  }
  sc_object* create ( const string& name ) {
    string hierach_name = getHierarchicalName(name);
    if (scml_property_registry::inst().hasProperty(scml_property_registry::MODULE, scml_property_registry::BOOL, hierach_name, "runtime_disabled") && 
        scml_property_registry::inst().getBoolProperty(scml_property_registry::MODULE, hierach_name, "runtime_disabled")) {
      sc_module_name n(name.c_str());
      if (creationVerboseMode() >= 6) { std::cout << "Monitor/Monitor: STUB for " << hierach_name << " created." << std::endl; }
      conf::stub *result = new conf::stub(n);
       conf::stub_port_registrator<>::register_stub_port(&Monitor::clk, "clk" , string(static_cast<sc_object*>(result)->name()) + ".clk" );
       conf::stub_port_registrator<>::register_stub_port(&Monitor::rst, "rst" , string(static_cast<sc_object*>(result)->name()) + ".rst" );
       conf::stub_port_registrator<>::register_stub_port(&Monitor::data_in, "data_in" , string(static_cast<sc_object*>(result)->name()) + ".data_in" );
       conf::stub_port_registrator<>::register_stub_port(&Monitor::data_valid, "data_valid" , string(static_cast<sc_object*>(result)->name()) + ".data_valid" );
      return result;
    } else {
      if (creationVerboseMode() >= 6) { std::cout << "Monitor/Monitor: " << hierach_name << " created." << std::endl; }
      Monitor* result = new Monitor(name.c_str());
       cwr_sc_object_registry::inst().addPort(&result->clk, string(static_cast<sc_object*>(result)->name()) + ".clk" );
       cwr_sc_object_registry::inst().addPort(&result->rst, string(static_cast<sc_object*>(result)->name()) + ".rst" );
       cwr_sc_object_registry::inst().addPort(&result->data_in, string(static_cast<sc_object*>(result)->name()) + ".data_in" );
       cwr_sc_object_registry::inst().addPort(&result->data_valid, string(static_cast<sc_object*>(result)->name()) + ".data_valid" );
      return result;
    }
  }
};


class Network_Interface0Creator : public ScObjectCreatorBase
{
public:
  static unsigned int creationVerboseMode() {
    const char * const env_var_val = ::getenv("SNPS_SLS_DYNAMIC_CREATION_VERBOSE");
    return env_var_val ? (::atoi(env_var_val)) : 3;
  }
  sc_object* create ( const string& name ) {
    string hierach_name = getHierarchicalName(name);
    int _lx = scml_property_registry::inst().getIntProperty(
	      scml_property_registry::CONSTRUCTOR, hierach_name, "_lx");

    int _ly = scml_property_registry::inst().getIntProperty(
	      scml_property_registry::CONSTRUCTOR, hierach_name, "_ly");

    if (scml_property_registry::inst().hasProperty(scml_property_registry::MODULE, scml_property_registry::BOOL, hierach_name, "runtime_disabled") && 
        scml_property_registry::inst().getBoolProperty(scml_property_registry::MODULE, hierach_name, "runtime_disabled")) {
      sc_module_name n(name.c_str());
      if (creationVerboseMode() >= 6) { std::cout << "Network_Interface/Network_Interface: STUB for " << hierach_name << " created." << std::endl; }
      conf::stub *result = new conf::stub(n);
       conf::stub_port_registrator<>::register_stub_port(&Network_Interface::clk, "clk" , string(static_cast<sc_object*>(result)->name()) + ".clk" );
       conf::stub_port_registrator<>::register_stub_port(&Network_Interface::reset, "reset" , string(static_cast<sc_object*>(result)->name()) + ".reset" );
       conf::stub_port_registrator<>::register_stub_port(&Network_Interface::Flit_in, "Flit_in" , string(static_cast<sc_object*>(result)->name()) + ".Flit_in" );
       conf::stub_port_registrator<>::register_stub_port(&Network_Interface::Data_in, "Data_in" , string(static_cast<sc_object*>(result)->name()) + ".Data_in" );
       conf::stub_port_registrator<>::register_stub_port(&Network_Interface::Flit_out, "Flit_out" , string(static_cast<sc_object*>(result)->name()) + ".Flit_out" );
       conf::stub_port_registrator<>::register_stub_port(&Network_Interface::Data_out, "Data_out" , string(static_cast<sc_object*>(result)->name()) + ".Data_out" );
       conf::stub_port_registrator<>::register_stub_port(&Network_Interface::L_req_in, "L_req_in" , string(static_cast<sc_object*>(result)->name()) + ".L_req_in" );
       conf::stub_port_registrator<>::register_stub_port(&Network_Interface::L_ack_out, "L_ack_out" , string(static_cast<sc_object*>(result)->name()) + ".L_ack_out" );
       conf::stub_port_registrator<>::register_stub_port(&Network_Interface::L_ack_in, "L_ack_in" , string(static_cast<sc_object*>(result)->name()) + ".L_ack_in" );
       conf::stub_port_registrator<>::register_stub_port(&Network_Interface::L_req_out, "L_req_out" , string(static_cast<sc_object*>(result)->name()) + ".L_req_out" );
       conf::stub_port_registrator<>::register_stub_port(&Network_Interface::PE_req_in, "PE_req_in" , string(static_cast<sc_object*>(result)->name()) + ".PE_req_in" );
       conf::stub_port_registrator<>::register_stub_port(&Network_Interface::PE_ack_out, "PE_ack_out" , string(static_cast<sc_object*>(result)->name()) + ".PE_ack_out" );
       conf::stub_port_registrator<>::register_stub_port(&Network_Interface::PE_ack_in, "PE_ack_in" , string(static_cast<sc_object*>(result)->name()) + ".PE_ack_in" );
       conf::stub_port_registrator<>::register_stub_port(&Network_Interface::PE_req_out, "PE_req_out" , string(static_cast<sc_object*>(result)->name()) + ".PE_req_out" );
      return result;
    } else {
      if (creationVerboseMode() >= 6) { std::cout << "Network_Interface/Network_Interface: " << hierach_name << " created." << std::endl; }
      Network_Interface* result = new Network_Interface(name.c_str(), _lx, _ly);
       cwr_sc_object_registry::inst().addPort(&result->clk, string(static_cast<sc_object*>(result)->name()) + ".clk" );
       cwr_sc_object_registry::inst().addPort(&result->reset, string(static_cast<sc_object*>(result)->name()) + ".reset" );
       cwr_sc_object_registry::inst().addPort(&result->Flit_in, string(static_cast<sc_object*>(result)->name()) + ".Flit_in" );
       cwr_sc_object_registry::inst().addPort(&result->Data_in, string(static_cast<sc_object*>(result)->name()) + ".Data_in" );
       cwr_sc_object_registry::inst().addPort(&result->Flit_out, string(static_cast<sc_object*>(result)->name()) + ".Flit_out" );
       cwr_sc_object_registry::inst().addPort(&result->Data_out, string(static_cast<sc_object*>(result)->name()) + ".Data_out" );
       cwr_sc_object_registry::inst().addPort(&result->L_req_in, string(static_cast<sc_object*>(result)->name()) + ".L_req_in" );
       cwr_sc_object_registry::inst().addPort(&result->L_ack_out, string(static_cast<sc_object*>(result)->name()) + ".L_ack_out" );
       cwr_sc_object_registry::inst().addPort(&result->L_ack_in, string(static_cast<sc_object*>(result)->name()) + ".L_ack_in" );
       cwr_sc_object_registry::inst().addPort(&result->L_req_out, string(static_cast<sc_object*>(result)->name()) + ".L_req_out" );
       cwr_sc_object_registry::inst().addPort(&result->PE_req_in, string(static_cast<sc_object*>(result)->name()) + ".PE_req_in" );
       cwr_sc_object_registry::inst().addPort(&result->PE_ack_out, string(static_cast<sc_object*>(result)->name()) + ".PE_ack_out" );
       cwr_sc_object_registry::inst().addPort(&result->PE_ack_in, string(static_cast<sc_object*>(result)->name()) + ".PE_ack_in" );
       cwr_sc_object_registry::inst().addPort(&result->PE_req_out, string(static_cast<sc_object*>(result)->name()) + ".PE_req_out" );
      return result;
    }
  }
};


class Processing_Element0Creator : public ScObjectCreatorBase
{
public:
  static unsigned int creationVerboseMode() {
    const char * const env_var_val = ::getenv("SNPS_SLS_DYNAMIC_CREATION_VERBOSE");
    return env_var_val ? (::atoi(env_var_val)) : 3;
  }
  sc_object* create ( const string& name ) {
    string hierach_name = getHierarchicalName(name);
    int _lx = scml_property_registry::inst().getIntProperty(
	      scml_property_registry::CONSTRUCTOR, hierach_name, "_lx");

    int _ly = scml_property_registry::inst().getIntProperty(
	      scml_property_registry::CONSTRUCTOR, hierach_name, "_ly");

    int _layer = scml_property_registry::inst().getIntProperty(
	      scml_property_registry::CONSTRUCTOR, hierach_name, "_layer");

    if (scml_property_registry::inst().hasProperty(scml_property_registry::MODULE, scml_property_registry::BOOL, hierach_name, "runtime_disabled") && 
        scml_property_registry::inst().getBoolProperty(scml_property_registry::MODULE, hierach_name, "runtime_disabled")) {
      sc_module_name n(name.c_str());
      if (creationVerboseMode() >= 6) { std::cout << "Processing_Element/Processing_Element: STUB for " << hierach_name << " created." << std::endl; }
      conf::stub *result = new conf::stub(n);
       conf::stub_port_registrator<>::register_stub_port(&Processing_Element::clk, "clk" , string(static_cast<sc_object*>(result)->name()) + ".clk" );
       conf::stub_port_registrator<>::register_stub_port(&Processing_Element::reset, "reset" , string(static_cast<sc_object*>(result)->name()) + ".reset" );
       conf::stub_port_registrator<>::register_stub_port(&Processing_Element::P_data_out, "P_data_out" , string(static_cast<sc_object*>(result)->name()) + ".P_data_out" );
       conf::stub_port_registrator<>::register_stub_port(&Processing_Element::P_out_valid, "P_out_valid" , string(static_cast<sc_object*>(result)->name()) + ".P_out_valid" );
       conf::stub_port_registrator<>::register_stub_port(&Processing_Element::P_data_in, "P_data_in" , string(static_cast<sc_object*>(result)->name()) + ".P_data_in" );
       conf::stub_port_registrator<>::register_stub_port(&Processing_Element::P_in_valid, "P_in_valid" , string(static_cast<sc_object*>(result)->name()) + ".P_in_valid" );
       conf::stub_port_registrator<>::register_stub_port(&Processing_Element::load_done, "load_done" , string(static_cast<sc_object*>(result)->name()) + ".load_done" );
       conf::stub_port_registrator<>::register_stub_port(&Processing_Element::Data_in, "Data_in" , string(static_cast<sc_object*>(result)->name()) + ".Data_in" );
       conf::stub_port_registrator<>::register_stub_port(&Processing_Element::Data_out, "Data_out" , string(static_cast<sc_object*>(result)->name()) + ".Data_out" );
       conf::stub_port_registrator<>::register_stub_port(&Processing_Element::PE_req_in, "PE_req_in" , string(static_cast<sc_object*>(result)->name()) + ".PE_req_in" );
       conf::stub_port_registrator<>::register_stub_port(&Processing_Element::PE_ack_out, "PE_ack_out" , string(static_cast<sc_object*>(result)->name()) + ".PE_ack_out" );
       conf::stub_port_registrator<>::register_stub_port(&Processing_Element::PE_ack_in, "PE_ack_in" , string(static_cast<sc_object*>(result)->name()) + ".PE_ack_in" );
       conf::stub_port_registrator<>::register_stub_port(&Processing_Element::PE_req_out, "PE_req_out" , string(static_cast<sc_object*>(result)->name()) + ".PE_req_out" );
      return result;
    } else {
      if (creationVerboseMode() >= 6) { std::cout << "Processing_Element/Processing_Element: " << hierach_name << " created." << std::endl; }
      Processing_Element* result = new Processing_Element(name.c_str(), _lx, _ly, _layer);
       cwr_sc_object_registry::inst().addPort(&result->clk, string(static_cast<sc_object*>(result)->name()) + ".clk" );
       cwr_sc_object_registry::inst().addPort(&result->reset, string(static_cast<sc_object*>(result)->name()) + ".reset" );
       cwr_sc_object_registry::inst().addPort(&result->P_data_out, string(static_cast<sc_object*>(result)->name()) + ".P_data_out" );
       cwr_sc_object_registry::inst().addPort(&result->P_out_valid, string(static_cast<sc_object*>(result)->name()) + ".P_out_valid" );
       cwr_sc_object_registry::inst().addPort(&result->P_data_in, string(static_cast<sc_object*>(result)->name()) + ".P_data_in" );
       cwr_sc_object_registry::inst().addPort(&result->P_in_valid, string(static_cast<sc_object*>(result)->name()) + ".P_in_valid" );
       cwr_sc_object_registry::inst().addPort(&result->load_done, string(static_cast<sc_object*>(result)->name()) + ".load_done" );
       cwr_sc_object_registry::inst().addPort(&result->Data_in, string(static_cast<sc_object*>(result)->name()) + ".Data_in" );
       cwr_sc_object_registry::inst().addPort(&result->Data_out, string(static_cast<sc_object*>(result)->name()) + ".Data_out" );
       cwr_sc_object_registry::inst().addPort(&result->PE_req_in, string(static_cast<sc_object*>(result)->name()) + ".PE_req_in" );
       cwr_sc_object_registry::inst().addPort(&result->PE_ack_out, string(static_cast<sc_object*>(result)->name()) + ".PE_ack_out" );
       cwr_sc_object_registry::inst().addPort(&result->PE_ack_in, string(static_cast<sc_object*>(result)->name()) + ".PE_ack_in" );
       cwr_sc_object_registry::inst().addPort(&result->PE_req_out, string(static_cast<sc_object*>(result)->name()) + ".PE_req_out" );
      return result;
    }
  }
};


class RAM0Creator : public ScObjectCreatorBase
{
public:
  static unsigned int creationVerboseMode() {
    const char * const env_var_val = ::getenv("SNPS_SLS_DYNAMIC_CREATION_VERBOSE");
    return env_var_val ? (::atoi(env_var_val)) : 3;
  }
  sc_object* create ( const string& name ) {
    string hierach_name = getHierarchicalName(name);
    if (scml_property_registry::inst().hasProperty(scml_property_registry::MODULE, scml_property_registry::BOOL, hierach_name, "runtime_disabled") && 
        scml_property_registry::inst().getBoolProperty(scml_property_registry::MODULE, hierach_name, "runtime_disabled")) {
      sc_module_name n(name.c_str());
      if (creationVerboseMode() >= 6) { std::cout << "RAM/RAM: STUB for " << hierach_name << " created." << std::endl; }
      conf::stub *result = new conf::stub(n);
       conf::stub_port_registrator<>::register_stub_port(&RAM::clk, "clk" , string(static_cast<sc_object*>(result)->name()) + ".clk" );
       conf::stub_port_registrator<>::register_stub_port(&RAM::wr, "wr" , string(static_cast<sc_object*>(result)->name()) + ".wr" );
       conf::stub_port_registrator<>::register_stub_port(&RAM::addr, "addr" , string(static_cast<sc_object*>(result)->name()) + ".addr" );
       conf::stub_port_registrator<>::register_stub_port(&RAM::data_out, "data_out" , string(static_cast<sc_object*>(result)->name()) + ".data_out" );
       conf::stub_port_registrator<>::register_stub_port(&RAM::data_in, "data_in" , string(static_cast<sc_object*>(result)->name()) + ".data_in" );
      return result;
    } else {
      if (creationVerboseMode() >= 6) { std::cout << "RAM/RAM: " << hierach_name << " created." << std::endl; }
      RAM* result = new RAM(name.c_str());
       cwr_sc_object_registry::inst().addPort(&result->clk, string(static_cast<sc_object*>(result)->name()) + ".clk" );
       cwr_sc_object_registry::inst().addPort(&result->wr, string(static_cast<sc_object*>(result)->name()) + ".wr" );
       cwr_sc_object_registry::inst().addPort(&result->addr, string(static_cast<sc_object*>(result)->name()) + ".addr" );
       cwr_sc_object_registry::inst().addPort(&result->data_out, string(static_cast<sc_object*>(result)->name()) + ".data_out" );
       cwr_sc_object_registry::inst().addPort(&result->data_in, string(static_cast<sc_object*>(result)->name()) + ".data_in" );
      return result;
    }
  }
};


class ROM0Creator : public ScObjectCreatorBase
{
public:
  static unsigned int creationVerboseMode() {
    const char * const env_var_val = ::getenv("SNPS_SLS_DYNAMIC_CREATION_VERBOSE");
    return env_var_val ? (::atoi(env_var_val)) : 3;
  }
  sc_object* create ( const string& name ) {
    string hierach_name = getHierarchicalName(name);
    if (scml_property_registry::inst().hasProperty(scml_property_registry::MODULE, scml_property_registry::BOOL, hierach_name, "runtime_disabled") && 
        scml_property_registry::inst().getBoolProperty(scml_property_registry::MODULE, hierach_name, "runtime_disabled")) {
      sc_module_name n(name.c_str());
      if (creationVerboseMode() >= 6) { std::cout << "ROM/ROM: STUB for " << hierach_name << " created." << std::endl; }
      conf::stub *result = new conf::stub(n);
       conf::stub_port_registrator<>::register_stub_port(&ROM::clk, "clk" , string(static_cast<sc_object*>(result)->name()) + ".clk" );
       conf::stub_port_registrator<>::register_stub_port(&ROM::ird, "ird" , string(static_cast<sc_object*>(result)->name()) + ".ird" );
       conf::stub_port_registrator<>::register_stub_port(&ROM::addr, "addr" , string(static_cast<sc_object*>(result)->name()) + ".addr" );
       conf::stub_port_registrator<>::register_stub_port(&ROM::data_out, "data_out" , string(static_cast<sc_object*>(result)->name()) + ".data_out" );
      return result;
    } else {
      if (creationVerboseMode() >= 6) { std::cout << "ROM/ROM: " << hierach_name << " created." << std::endl; }
      ROM* result = new ROM(name.c_str());
       cwr_sc_object_registry::inst().addPort(&result->clk, string(static_cast<sc_object*>(result)->name()) + ".clk" );
       cwr_sc_object_registry::inst().addPort(&result->ird, string(static_cast<sc_object*>(result)->name()) + ".ird" );
       cwr_sc_object_registry::inst().addPort(&result->addr, string(static_cast<sc_object*>(result)->name()) + ".addr" );
       cwr_sc_object_registry::inst().addPort(&result->data_out, string(static_cast<sc_object*>(result)->name()) + ".data_out" );
      return result;
    }
  }
};


class Reset0Creator : public ScObjectCreatorBase
{
public:
  static unsigned int creationVerboseMode() {
    const char * const env_var_val = ::getenv("SNPS_SLS_DYNAMIC_CREATION_VERBOSE");
    return env_var_val ? (::atoi(env_var_val)) : 3;
  }
  sc_object* create ( const string& name ) {
    string hierach_name = getHierarchicalName(name);
    int _ticks = scml_property_registry::inst().getIntProperty(
	      scml_property_registry::CONSTRUCTOR, hierach_name, "_ticks");

    if (scml_property_registry::inst().hasProperty(scml_property_registry::MODULE, scml_property_registry::BOOL, hierach_name, "runtime_disabled") && 
        scml_property_registry::inst().getBoolProperty(scml_property_registry::MODULE, hierach_name, "runtime_disabled")) {
      sc_module_name n(name.c_str());
      if (creationVerboseMode() >= 6) { std::cout << "Reset/Reset: STUB for " << hierach_name << " created." << std::endl; }
      conf::stub *result = new conf::stub(n);
       conf::stub_port_registrator<>::register_stub_port(&Reset::rst, "rst" , string(static_cast<sc_object*>(result)->name()) + ".rst" );
      return result;
    } else {
      if (creationVerboseMode() >= 6) { std::cout << "Reset/Reset: " << hierach_name << " created." << std::endl; }
      Reset* result = new Reset(name.c_str(), _ticks);
       cwr_sc_object_registry::inst().addPort(&result->rst, string(static_cast<sc_object*>(result)->name()) + ".rst" );
      return result;
    }
  }
};


class Router0Creator : public ScObjectCreatorBase
{
public:
  static unsigned int creationVerboseMode() {
    const char * const env_var_val = ::getenv("SNPS_SLS_DYNAMIC_CREATION_VERBOSE");
    return env_var_val ? (::atoi(env_var_val)) : 3;
  }
  sc_object* create ( const string& name ) {
    string hierach_name = getHierarchicalName(name);
    int _lx = scml_property_registry::inst().getIntProperty(
	      scml_property_registry::CONSTRUCTOR, hierach_name, "_lx");

    int _ly = scml_property_registry::inst().getIntProperty(
	      scml_property_registry::CONSTRUCTOR, hierach_name, "_ly");

    if (scml_property_registry::inst().hasProperty(scml_property_registry::MODULE, scml_property_registry::BOOL, hierach_name, "runtime_disabled") && 
        scml_property_registry::inst().getBoolProperty(scml_property_registry::MODULE, hierach_name, "runtime_disabled")) {
      sc_module_name n(name.c_str());
      if (creationVerboseMode() >= 6) { std::cout << "Router/Router: STUB for " << hierach_name << " created." << std::endl; }
      conf::stub *result = new conf::stub(n);
       conf::stub_port_registrator<>::register_stub_port(&Router::clk, "clk" , string(static_cast<sc_object*>(result)->name()) + ".clk" );
       conf::stub_port_registrator<>::register_stub_port(&Router::reset, "reset" , string(static_cast<sc_object*>(result)->name()) + ".reset" );
       conf::stub_port_registrator<>::register_stub_port(&Router::N_data_in, "N_data_in" , string(static_cast<sc_object*>(result)->name()) + ".N_data_in" );
       conf::stub_port_registrator<>::register_stub_port(&Router::W_data_in, "W_data_in" , string(static_cast<sc_object*>(result)->name()) + ".W_data_in" );
       conf::stub_port_registrator<>::register_stub_port(&Router::S_data_in, "S_data_in" , string(static_cast<sc_object*>(result)->name()) + ".S_data_in" );
       conf::stub_port_registrator<>::register_stub_port(&Router::E_data_in, "E_data_in" , string(static_cast<sc_object*>(result)->name()) + ".E_data_in" );
       conf::stub_port_registrator<>::register_stub_port(&Router::L_data_in, "L_data_in" , string(static_cast<sc_object*>(result)->name()) + ".L_data_in" );
       conf::stub_port_registrator<>::register_stub_port(&Router::N_data_out, "N_data_out" , string(static_cast<sc_object*>(result)->name()) + ".N_data_out" );
       conf::stub_port_registrator<>::register_stub_port(&Router::W_data_out, "W_data_out" , string(static_cast<sc_object*>(result)->name()) + ".W_data_out" );
       conf::stub_port_registrator<>::register_stub_port(&Router::S_data_out, "S_data_out" , string(static_cast<sc_object*>(result)->name()) + ".S_data_out" );
       conf::stub_port_registrator<>::register_stub_port(&Router::E_data_out, "E_data_out" , string(static_cast<sc_object*>(result)->name()) + ".E_data_out" );
       conf::stub_port_registrator<>::register_stub_port(&Router::L_data_out, "L_data_out" , string(static_cast<sc_object*>(result)->name()) + ".L_data_out" );
       conf::stub_port_registrator<>::register_stub_port(&Router::N_req_in, "N_req_in" , string(static_cast<sc_object*>(result)->name()) + ".N_req_in" );
       conf::stub_port_registrator<>::register_stub_port(&Router::W_req_in, "W_req_in" , string(static_cast<sc_object*>(result)->name()) + ".W_req_in" );
       conf::stub_port_registrator<>::register_stub_port(&Router::S_req_in, "S_req_in" , string(static_cast<sc_object*>(result)->name()) + ".S_req_in" );
       conf::stub_port_registrator<>::register_stub_port(&Router::E_req_in, "E_req_in" , string(static_cast<sc_object*>(result)->name()) + ".E_req_in" );
       conf::stub_port_registrator<>::register_stub_port(&Router::L_req_in, "L_req_in" , string(static_cast<sc_object*>(result)->name()) + ".L_req_in" );
       conf::stub_port_registrator<>::register_stub_port(&Router::N_ack_out, "N_ack_out" , string(static_cast<sc_object*>(result)->name()) + ".N_ack_out" );
       conf::stub_port_registrator<>::register_stub_port(&Router::W_ack_out, "W_ack_out" , string(static_cast<sc_object*>(result)->name()) + ".W_ack_out" );
       conf::stub_port_registrator<>::register_stub_port(&Router::S_ack_out, "S_ack_out" , string(static_cast<sc_object*>(result)->name()) + ".S_ack_out" );
       conf::stub_port_registrator<>::register_stub_port(&Router::E_ack_out, "E_ack_out" , string(static_cast<sc_object*>(result)->name()) + ".E_ack_out" );
       conf::stub_port_registrator<>::register_stub_port(&Router::L_ack_out, "L_ack_out" , string(static_cast<sc_object*>(result)->name()) + ".L_ack_out" );
       conf::stub_port_registrator<>::register_stub_port(&Router::N_ack_in, "N_ack_in" , string(static_cast<sc_object*>(result)->name()) + ".N_ack_in" );
       conf::stub_port_registrator<>::register_stub_port(&Router::W_ack_in, "W_ack_in" , string(static_cast<sc_object*>(result)->name()) + ".W_ack_in" );
       conf::stub_port_registrator<>::register_stub_port(&Router::S_ack_in, "S_ack_in" , string(static_cast<sc_object*>(result)->name()) + ".S_ack_in" );
       conf::stub_port_registrator<>::register_stub_port(&Router::E_ack_in, "E_ack_in" , string(static_cast<sc_object*>(result)->name()) + ".E_ack_in" );
       conf::stub_port_registrator<>::register_stub_port(&Router::L_ack_in, "L_ack_in" , string(static_cast<sc_object*>(result)->name()) + ".L_ack_in" );
       conf::stub_port_registrator<>::register_stub_port(&Router::N_req_out, "N_req_out" , string(static_cast<sc_object*>(result)->name()) + ".N_req_out" );
       conf::stub_port_registrator<>::register_stub_port(&Router::W_req_out, "W_req_out" , string(static_cast<sc_object*>(result)->name()) + ".W_req_out" );
       conf::stub_port_registrator<>::register_stub_port(&Router::S_req_out, "S_req_out" , string(static_cast<sc_object*>(result)->name()) + ".S_req_out" );
       conf::stub_port_registrator<>::register_stub_port(&Router::E_req_out, "E_req_out" , string(static_cast<sc_object*>(result)->name()) + ".E_req_out" );
       conf::stub_port_registrator<>::register_stub_port(&Router::L_req_out, "L_req_out" , string(static_cast<sc_object*>(result)->name()) + ".L_req_out" );
      return result;
    } else {
      if (creationVerboseMode() >= 6) { std::cout << "Router/Router: " << hierach_name << " created." << std::endl; }
      Router* result = new Router(name.c_str(), _lx, _ly);
       cwr_sc_object_registry::inst().addPort(&result->clk, string(static_cast<sc_object*>(result)->name()) + ".clk" );
       cwr_sc_object_registry::inst().addPort(&result->reset, string(static_cast<sc_object*>(result)->name()) + ".reset" );
       cwr_sc_object_registry::inst().addPort(&result->N_data_in, string(static_cast<sc_object*>(result)->name()) + ".N_data_in" );
       cwr_sc_object_registry::inst().addPort(&result->W_data_in, string(static_cast<sc_object*>(result)->name()) + ".W_data_in" );
       cwr_sc_object_registry::inst().addPort(&result->S_data_in, string(static_cast<sc_object*>(result)->name()) + ".S_data_in" );
       cwr_sc_object_registry::inst().addPort(&result->E_data_in, string(static_cast<sc_object*>(result)->name()) + ".E_data_in" );
       cwr_sc_object_registry::inst().addPort(&result->L_data_in, string(static_cast<sc_object*>(result)->name()) + ".L_data_in" );
       cwr_sc_object_registry::inst().addPort(&result->N_data_out, string(static_cast<sc_object*>(result)->name()) + ".N_data_out" );
       cwr_sc_object_registry::inst().addPort(&result->W_data_out, string(static_cast<sc_object*>(result)->name()) + ".W_data_out" );
       cwr_sc_object_registry::inst().addPort(&result->S_data_out, string(static_cast<sc_object*>(result)->name()) + ".S_data_out" );
       cwr_sc_object_registry::inst().addPort(&result->E_data_out, string(static_cast<sc_object*>(result)->name()) + ".E_data_out" );
       cwr_sc_object_registry::inst().addPort(&result->L_data_out, string(static_cast<sc_object*>(result)->name()) + ".L_data_out" );
       cwr_sc_object_registry::inst().addPort(&result->N_req_in, string(static_cast<sc_object*>(result)->name()) + ".N_req_in" );
       cwr_sc_object_registry::inst().addPort(&result->W_req_in, string(static_cast<sc_object*>(result)->name()) + ".W_req_in" );
       cwr_sc_object_registry::inst().addPort(&result->S_req_in, string(static_cast<sc_object*>(result)->name()) + ".S_req_in" );
       cwr_sc_object_registry::inst().addPort(&result->E_req_in, string(static_cast<sc_object*>(result)->name()) + ".E_req_in" );
       cwr_sc_object_registry::inst().addPort(&result->L_req_in, string(static_cast<sc_object*>(result)->name()) + ".L_req_in" );
       cwr_sc_object_registry::inst().addPort(&result->N_ack_out, string(static_cast<sc_object*>(result)->name()) + ".N_ack_out" );
       cwr_sc_object_registry::inst().addPort(&result->W_ack_out, string(static_cast<sc_object*>(result)->name()) + ".W_ack_out" );
       cwr_sc_object_registry::inst().addPort(&result->S_ack_out, string(static_cast<sc_object*>(result)->name()) + ".S_ack_out" );
       cwr_sc_object_registry::inst().addPort(&result->E_ack_out, string(static_cast<sc_object*>(result)->name()) + ".E_ack_out" );
       cwr_sc_object_registry::inst().addPort(&result->L_ack_out, string(static_cast<sc_object*>(result)->name()) + ".L_ack_out" );
       cwr_sc_object_registry::inst().addPort(&result->N_ack_in, string(static_cast<sc_object*>(result)->name()) + ".N_ack_in" );
       cwr_sc_object_registry::inst().addPort(&result->W_ack_in, string(static_cast<sc_object*>(result)->name()) + ".W_ack_in" );
       cwr_sc_object_registry::inst().addPort(&result->S_ack_in, string(static_cast<sc_object*>(result)->name()) + ".S_ack_in" );
       cwr_sc_object_registry::inst().addPort(&result->E_ack_in, string(static_cast<sc_object*>(result)->name()) + ".E_ack_in" );
       cwr_sc_object_registry::inst().addPort(&result->L_ack_in, string(static_cast<sc_object*>(result)->name()) + ".L_ack_in" );
       cwr_sc_object_registry::inst().addPort(&result->N_req_out, string(static_cast<sc_object*>(result)->name()) + ".N_req_out" );
       cwr_sc_object_registry::inst().addPort(&result->W_req_out, string(static_cast<sc_object*>(result)->name()) + ".W_req_out" );
       cwr_sc_object_registry::inst().addPort(&result->S_req_out, string(static_cast<sc_object*>(result)->name()) + ".S_req_out" );
       cwr_sc_object_registry::inst().addPort(&result->E_req_out, string(static_cast<sc_object*>(result)->name()) + ".E_req_out" );
       cwr_sc_object_registry::inst().addPort(&result->L_req_out, string(static_cast<sc_object*>(result)->name()) + ".L_req_out" );
      return result;
    }
  }
};


class Tile0Creator : public ScObjectCreatorBase
{
public:
  static unsigned int creationVerboseMode() {
    const char * const env_var_val = ::getenv("SNPS_SLS_DYNAMIC_CREATION_VERBOSE");
    return env_var_val ? (::atoi(env_var_val)) : 3;
  }
  sc_object* create ( const string& name ) {
    string hierach_name = getHierarchicalName(name);
    int _lx = scml_property_registry::inst().getIntProperty(
	      scml_property_registry::CONSTRUCTOR, hierach_name, "_lx");

    int _ly = scml_property_registry::inst().getIntProperty(
	      scml_property_registry::CONSTRUCTOR, hierach_name, "_ly");

    int _layer = scml_property_registry::inst().getIntProperty(
	      scml_property_registry::CONSTRUCTOR, hierach_name, "_layer");

    if (scml_property_registry::inst().hasProperty(scml_property_registry::MODULE, scml_property_registry::BOOL, hierach_name, "runtime_disabled") && 
        scml_property_registry::inst().getBoolProperty(scml_property_registry::MODULE, hierach_name, "runtime_disabled")) {
      sc_module_name n(name.c_str());
      if (creationVerboseMode() >= 6) { std::cout << "Tile/Tile: STUB for " << hierach_name << " created." << std::endl; }
      conf::stub *result = new conf::stub(n);
       conf::stub_port_registrator<>::register_stub_port(&Tile::clk, "clk" , string(static_cast<sc_object*>(result)->name()) + ".clk" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::reset, "reset" , string(static_cast<sc_object*>(result)->name()) + ".reset" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::P_data_out, "P_data_out" , string(static_cast<sc_object*>(result)->name()) + ".P_data_out" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::P_out_valid, "P_out_valid" , string(static_cast<sc_object*>(result)->name()) + ".P_out_valid" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::P_data_in, "P_data_in" , string(static_cast<sc_object*>(result)->name()) + ".P_data_in" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::P_in_valid, "P_in_valid" , string(static_cast<sc_object*>(result)->name()) + ".P_in_valid" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::load_done, "load_done" , string(static_cast<sc_object*>(result)->name()) + ".load_done" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::N_data_in, "N_data_in" , string(static_cast<sc_object*>(result)->name()) + ".N_data_in" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::W_data_in, "W_data_in" , string(static_cast<sc_object*>(result)->name()) + ".W_data_in" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::S_data_in, "S_data_in" , string(static_cast<sc_object*>(result)->name()) + ".S_data_in" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::E_data_in, "E_data_in" , string(static_cast<sc_object*>(result)->name()) + ".E_data_in" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::N_data_out, "N_data_out" , string(static_cast<sc_object*>(result)->name()) + ".N_data_out" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::W_data_out, "W_data_out" , string(static_cast<sc_object*>(result)->name()) + ".W_data_out" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::S_data_out, "S_data_out" , string(static_cast<sc_object*>(result)->name()) + ".S_data_out" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::E_data_out, "E_data_out" , string(static_cast<sc_object*>(result)->name()) + ".E_data_out" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::N_req_in, "N_req_in" , string(static_cast<sc_object*>(result)->name()) + ".N_req_in" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::W_req_in, "W_req_in" , string(static_cast<sc_object*>(result)->name()) + ".W_req_in" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::S_req_in, "S_req_in" , string(static_cast<sc_object*>(result)->name()) + ".S_req_in" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::E_req_in, "E_req_in" , string(static_cast<sc_object*>(result)->name()) + ".E_req_in" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::N_ack_out, "N_ack_out" , string(static_cast<sc_object*>(result)->name()) + ".N_ack_out" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::W_ack_out, "W_ack_out" , string(static_cast<sc_object*>(result)->name()) + ".W_ack_out" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::S_ack_out, "S_ack_out" , string(static_cast<sc_object*>(result)->name()) + ".S_ack_out" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::E_ack_out, "E_ack_out" , string(static_cast<sc_object*>(result)->name()) + ".E_ack_out" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::N_ack_in, "N_ack_in" , string(static_cast<sc_object*>(result)->name()) + ".N_ack_in" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::W_ack_in, "W_ack_in" , string(static_cast<sc_object*>(result)->name()) + ".W_ack_in" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::S_ack_in, "S_ack_in" , string(static_cast<sc_object*>(result)->name()) + ".S_ack_in" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::E_ack_in, "E_ack_in" , string(static_cast<sc_object*>(result)->name()) + ".E_ack_in" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::N_req_out, "N_req_out" , string(static_cast<sc_object*>(result)->name()) + ".N_req_out" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::W_req_out, "W_req_out" , string(static_cast<sc_object*>(result)->name()) + ".W_req_out" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::S_req_out, "S_req_out" , string(static_cast<sc_object*>(result)->name()) + ".S_req_out" );
       conf::stub_port_registrator<>::register_stub_port(&Tile::E_req_out, "E_req_out" , string(static_cast<sc_object*>(result)->name()) + ".E_req_out" );
      return result;
    } else {
      if (creationVerboseMode() >= 6) { std::cout << "Tile/Tile: " << hierach_name << " created." << std::endl; }
      Tile* result = new Tile(name.c_str(), _lx, _ly, _layer);
       cwr_sc_object_registry::inst().addPort(&result->clk, string(static_cast<sc_object*>(result)->name()) + ".clk" );
       cwr_sc_object_registry::inst().addPort(&result->reset, string(static_cast<sc_object*>(result)->name()) + ".reset" );
       cwr_sc_object_registry::inst().addPort(&result->P_data_out, string(static_cast<sc_object*>(result)->name()) + ".P_data_out" );
       cwr_sc_object_registry::inst().addPort(&result->P_out_valid, string(static_cast<sc_object*>(result)->name()) + ".P_out_valid" );
       cwr_sc_object_registry::inst().addPort(&result->P_data_in, string(static_cast<sc_object*>(result)->name()) + ".P_data_in" );
       cwr_sc_object_registry::inst().addPort(&result->P_in_valid, string(static_cast<sc_object*>(result)->name()) + ".P_in_valid" );
       cwr_sc_object_registry::inst().addPort(&result->load_done, string(static_cast<sc_object*>(result)->name()) + ".load_done" );
       cwr_sc_object_registry::inst().addPort(&result->N_data_in, string(static_cast<sc_object*>(result)->name()) + ".N_data_in" );
       cwr_sc_object_registry::inst().addPort(&result->W_data_in, string(static_cast<sc_object*>(result)->name()) + ".W_data_in" );
       cwr_sc_object_registry::inst().addPort(&result->S_data_in, string(static_cast<sc_object*>(result)->name()) + ".S_data_in" );
       cwr_sc_object_registry::inst().addPort(&result->E_data_in, string(static_cast<sc_object*>(result)->name()) + ".E_data_in" );
       cwr_sc_object_registry::inst().addPort(&result->N_data_out, string(static_cast<sc_object*>(result)->name()) + ".N_data_out" );
       cwr_sc_object_registry::inst().addPort(&result->W_data_out, string(static_cast<sc_object*>(result)->name()) + ".W_data_out" );
       cwr_sc_object_registry::inst().addPort(&result->S_data_out, string(static_cast<sc_object*>(result)->name()) + ".S_data_out" );
       cwr_sc_object_registry::inst().addPort(&result->E_data_out, string(static_cast<sc_object*>(result)->name()) + ".E_data_out" );
       cwr_sc_object_registry::inst().addPort(&result->N_req_in, string(static_cast<sc_object*>(result)->name()) + ".N_req_in" );
       cwr_sc_object_registry::inst().addPort(&result->W_req_in, string(static_cast<sc_object*>(result)->name()) + ".W_req_in" );
       cwr_sc_object_registry::inst().addPort(&result->S_req_in, string(static_cast<sc_object*>(result)->name()) + ".S_req_in" );
       cwr_sc_object_registry::inst().addPort(&result->E_req_in, string(static_cast<sc_object*>(result)->name()) + ".E_req_in" );
       cwr_sc_object_registry::inst().addPort(&result->N_ack_out, string(static_cast<sc_object*>(result)->name()) + ".N_ack_out" );
       cwr_sc_object_registry::inst().addPort(&result->W_ack_out, string(static_cast<sc_object*>(result)->name()) + ".W_ack_out" );
       cwr_sc_object_registry::inst().addPort(&result->S_ack_out, string(static_cast<sc_object*>(result)->name()) + ".S_ack_out" );
       cwr_sc_object_registry::inst().addPort(&result->E_ack_out, string(static_cast<sc_object*>(result)->name()) + ".E_ack_out" );
       cwr_sc_object_registry::inst().addPort(&result->N_ack_in, string(static_cast<sc_object*>(result)->name()) + ".N_ack_in" );
       cwr_sc_object_registry::inst().addPort(&result->W_ack_in, string(static_cast<sc_object*>(result)->name()) + ".W_ack_in" );
       cwr_sc_object_registry::inst().addPort(&result->S_ack_in, string(static_cast<sc_object*>(result)->name()) + ".S_ack_in" );
       cwr_sc_object_registry::inst().addPort(&result->E_ack_in, string(static_cast<sc_object*>(result)->name()) + ".E_ack_in" );
       cwr_sc_object_registry::inst().addPort(&result->N_req_out, string(static_cast<sc_object*>(result)->name()) + ".N_req_out" );
       cwr_sc_object_registry::inst().addPort(&result->W_req_out, string(static_cast<sc_object*>(result)->name()) + ".W_req_out" );
       cwr_sc_object_registry::inst().addPort(&result->S_req_out, string(static_cast<sc_object*>(result)->name()) + ".S_req_out" );
       cwr_sc_object_registry::inst().addPort(&result->E_req_out, string(static_cast<sc_object*>(result)->name()) + ".E_req_out" );
      return result;
    }
  }
};



extern void scv_tr_cwrdb_init();

int sc_main(int argc, char* argv[])
{
  scv_tr_cwrdb_init();
  ScObjectFactory::inst().addCreator ("Clock0", new Clock0Creator());
  ScObjectFactory::inst().addCreator ("LeNet0", new LeNet0Creator());
  ScObjectFactory::inst().addCreator ("Monitor0", new Monitor0Creator());
  ScObjectFactory::inst().addCreator ("Network_Interface0", new Network_Interface0Creator());
  ScObjectFactory::inst().addCreator ("Processing_Element0", new Processing_Element0Creator());
  ScObjectFactory::inst().addCreator ("RAM0", new RAM0Creator());
  ScObjectFactory::inst().addCreator ("ROM0", new ROM0Creator());
  ScObjectFactory::inst().addCreator ("Reset0", new Reset0Creator());
  ScObjectFactory::inst().addCreator ("Router0", new Router0Creator());
  ScObjectFactory::inst().addCreator ("Tile0", new Tile0Creator());
  ScObjectFactory::inst().addCreator ("sc_signal<bool>", new ScPrimChannelCreator<sc_signal<bool> >());
  ScObjectFactory::inst().addCreator ("sc_signal<sc_int<16> >", new ScPrimChannelCreator<sc_signal<sc_int<16> > >());
  ScObjectFactory::inst().addCreator ("sc_signal<sc_lv<16> >", new ScPrimChannelCreator<sc_signal<sc_lv<16> > >());
  ScObjectFactory::inst().addCreator ("sc_signal<sc_lv<22> >", new ScPrimChannelCreator<sc_signal<sc_lv<22> > >());
  ScObjectFactory::inst().addCreator ("sc_signal<sc_lv<32> >", new ScPrimChannelCreator<sc_signal<sc_lv<32> > >());
  ScObjectFactory::inst().addCreator ("sc_signal<sc_uint<16> >", new ScPrimChannelCreator<sc_signal<sc_uint<16> > >());
  ScPortFactory::inst().addCreator ("sc_in<bool>", new ScPortCreator<sc_in<bool> >());
  ScPortFactory::inst().addCreator ("sc_in<sc_int<16> >", new ScPortCreator<sc_in<sc_int<16> > >());
  ScPortFactory::inst().addCreator ("sc_in<sc_lv<16> >", new ScPortCreator<sc_in<sc_lv<16> > >());
  ScPortFactory::inst().addCreator ("sc_in<sc_lv<22> >", new ScPortCreator<sc_in<sc_lv<22> > >());
  ScPortFactory::inst().addCreator ("sc_in<sc_lv<32> >", new ScPortCreator<sc_in<sc_lv<32> > >());
  ScPortFactory::inst().addCreator ("sc_in<sc_uint<16> >", new ScPortCreator<sc_in<sc_uint<16> > >());
  ScPortFactory::inst().addCreator ("sc_inout<bool>", new ScPortCreator<sc_inout<bool> >());
  ScPortFactory::inst().addCreator ("sc_inout<sc_int<16> >", new ScPortCreator<sc_inout<sc_int<16> > >());
  ScPortFactory::inst().addCreator ("sc_inout<sc_lv<16> >", new ScPortCreator<sc_inout<sc_lv<16> > >());
  ScPortFactory::inst().addCreator ("sc_inout<sc_lv<22> >", new ScPortCreator<sc_inout<sc_lv<22> > >());
  ScPortFactory::inst().addCreator ("sc_inout<sc_lv<32> >", new ScPortCreator<sc_inout<sc_lv<32> > >());
  ScPortFactory::inst().addCreator ("sc_inout<sc_uint<16> >", new ScPortCreator<sc_inout<sc_uint<16> > >());
  ScPortFactory::inst().addCreator ("sc_out<bool>", new ScPortCreator<sc_out<bool> >());
  ScPortFactory::inst().addCreator ("sc_out<sc_int<16> >", new ScPortCreator<sc_out<sc_int<16> > >());
  ScPortFactory::inst().addCreator ("sc_out<sc_lv<16> >", new ScPortCreator<sc_out<sc_lv<16> > >());
  ScPortFactory::inst().addCreator ("sc_out<sc_lv<22> >", new ScPortCreator<sc_out<sc_lv<22> > >());
  ScPortFactory::inst().addCreator ("sc_out<sc_lv<32> >", new ScPortCreator<sc_out<sc_lv<32> > >());
  ScPortFactory::inst().addCreator ("sc_out<sc_uint<16> >", new ScPortCreator<sc_out<sc_uint<16> > >());
  if (::getenv("SNPS_SLS_DYNAMIC_LOADER_VERBOSE")) { std::cout << "Clock/Clock loaded." << std::endl; }
  if (::getenv("SNPS_SLS_DYNAMIC_LOADER_VERBOSE")) { std::cout << "LeNet/LeNet loaded." << std::endl; }
  if (::getenv("SNPS_SLS_DYNAMIC_LOADER_VERBOSE")) { std::cout << "Monitor/Monitor loaded." << std::endl; }
  if (::getenv("SNPS_SLS_DYNAMIC_LOADER_VERBOSE")) { std::cout << "Network_Interface/Network_Interface loaded." << std::endl; }
  if (::getenv("SNPS_SLS_DYNAMIC_LOADER_VERBOSE")) { std::cout << "Processing_Element/Processing_Element loaded." << std::endl; }
  if (::getenv("SNPS_SLS_DYNAMIC_LOADER_VERBOSE")) { std::cout << "RAM/RAM loaded." << std::endl; }
  if (::getenv("SNPS_SLS_DYNAMIC_LOADER_VERBOSE")) { std::cout << "ROM/ROM loaded." << std::endl; }
  if (::getenv("SNPS_SLS_DYNAMIC_LOADER_VERBOSE")) { std::cout << "Reset/Reset loaded." << std::endl; }
  if (::getenv("SNPS_SLS_DYNAMIC_LOADER_VERBOSE")) { std::cout << "Router/Router loaded." << std::endl; }
  if (::getenv("SNPS_SLS_DYNAMIC_LOADER_VERBOSE")) { std::cout << "Tile/Tile loaded." << std::endl; }

  scml_property_registry::inst().setXMLFile("Properties.xml");

  nSnps::nDynamicLoading::DynamicLoader dynamicLoader
    (scml_property_registry::inst().getStringProperty(scml_property_registry::GLOBAL, "", "/pct/runtime/dynamic_load_info"));

  string xml_file_path = scml_property_registry::inst().getPropertyXmlFilePath();
  string hierarchy_xml_file = xml_file_path + "/" + 
    scml_property_registry::inst().getStringProperty(
      scml_property_registry::GLOBAL, "", "system_hierarchy_xml_file");

  cwr_sc_hierarch_module* HARDWARE = new cwr_sc_hierarch_module("HARDWARE", hierarchy_xml_file);

  // Enable/disable backdoor accesses
  scml_abstraction_level_switch::instance().set_simulation_mode(scml_property_registry::inst().getStringProperty(scml_property_registry::GLOBAL, "", "/pct/runtime/simulation_mode")=="MODE_FULL_SIMULATION" ? scml_abstraction_level_switch::MODE_FULL_SIMULATION : scml_abstraction_level_switch::MODE_SPEED_OPTIMIZED);

  try {
    sc_start();
  } catch (const sc_core::sc_report& sce) { 
    std::cout << "Exception raised by sc_start() : " << sce.what() << std::endl;  
    return 1;  
  } 

  if (sc_is_running()) {
    sc_stop();
  }
  delete HARDWARE;

  return 0;
}
