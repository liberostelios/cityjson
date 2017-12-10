
#include <iostream>
#include <fstream>
#include "json.hpp"
#include <set>
#include <string>
#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

using json = nlohmann::json;

void xml_header();
void citygml_header();
void metadata();
void surface(std::vector<std::vector<int>> &onesurface);
void shell(json& jsh);
void multisurface(json& js);
void multisurface_w_semantics(json& js);
void compositesurface(json& js);
void solid(json& js);
void solid_w_semantics(json& js);
void geometry(json& jg, bool csasms);

void cityobject(std::string id, json& jco);
void building(std::string id, json& jb);
void waterbody(std::string id, json& jco);
void landuse(std::string id, json& jco);


//------------------------------------
// GLOBAL VARIABLES FOR CONVENIENCE
json j;
json jschema;
//------------------------------------


void xml_header() {
  std::cout << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
}

void citygml_header() {
  //-- boiler template for all files
  std::cout << "<CityModel xmlns=\"http://www.opengis.net/citygml/2.0\"\n";
  std::cout << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
  std::cout << "xmlns:xAL=\"urn:oasis:names:tc:ciq:xsdschema:xAL:2.0\"\n";
  std::cout << "xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n";
  std::cout << "xmlns:gml=\"http://www.opengis.net/gml\"\n";
  //-- if appearances
  if (j.count("appearance") != 0)
    std::cout << "xmlns:app=\"http://www.opengis.net/citygml/appearance/2.0\"\n";
  //-- collect all CO types
  std::set<std::string> d;
  for (auto& co : j["CityObjects"]) {
    std::string tmp = co["type"];
    d.insert(tmp);
  }
  //-- if there then put the XSD module
  if (d.count("Building") != 0)
    std::cout << "xmlns:bldg=\"http://www.opengis.net/citygml/building/2.0\"\n";
  if (d.count("WaterBody") != 0)
    std::cout << "xmlns:wtr=\"http://www.opengis.net/citygml/waterbody/2.0\"\n";
  if ( (d.count("PlantCover") != 0) || (d.count("SolitaryVegetationObject") != 0) )
    std::cout << "xmlns:veg=\"http://www.opengis.net/citygml/vegetation/2.0\"\n";
  if (d.count("TINRelief") != 0)
    std::cout << "xmlns:dem=\"http://www.opengis.net/citygml/relief/2.0\"\n";
  if ( (d.count("Road") != 0) || (d.count("Railway") != 0) || (d.count("TransportSquare") != 0) )
    std::cout << "xmlns:tran=\"http://www.opengis.net/citygml/transportation/2.0\"\n";
  if (d.count("LandUse") != 0)
    std::cout << "xmlns:luse=\"http://www.opengis.net/citygml/landuse/2.0\"\n";
  if (d.count("GenericCityObject") != 0)
    std::cout << "xmlns:gen=\"http://www.opengis.net/citygml/generics/2.0\"\n";
  if (d.count("Bridge") != 0)
    std::cout << "xmlns:brg=\"http://www.opengis.net/citygml/bridge/2.0\"\n";
  if (d.count("Tunnel") != 0)
    std::cout << "xmlns:tun=\"http://www.opengis.net/citygml/tunnel/2.0\"\n";
  if (d.count("CityFurniture") != 0)
    std::cout << "xmlns:cif=\"http://www.opengis.net/citygml/cityfurniture/2.0\"\n";
  std::cout << "xsi:schemaLocation=\"http://www.opengis.net/citygml/2.0 ./CityGML_2.0/CityGML.xsd\">\n";
}


void metadata() {
  if (j.count("metadata") != 0) {
    json jm = j["metadata"];
    if (jm.count("bbox") != 0) {
      std::cout << "<gml:boundedBy>" << std::endl;
      std::cout << "<gml:Envelope srsDimension=\"3\">" << std::endl;
      std::cout << "<gml:lowerCorner>" << std::endl;
      std::cout << jm["bbox"][0] << " " << jm["bbox"][1] << " " << jm["bbox"][2] << std::endl;
      std::cout << "</gml:lowerCorner>" << std::endl;
      std::cout << "<gml:upperCorner>" << std::endl;
      std::cout << jm["bbox"][3] << " " << jm["bbox"][4] << " " << jm["bbox"][5] << std::endl;
      std::cout << "</gml:upperCorner>" << std::endl;
      std::cout << "</gml:Envelope>" << std::endl;
      std::cout << "</gml:boundedBy>" << std::endl;
    }
    if (jm.count("datasetTitle") != 0) {
      std::cout << "<gml:name>" << jm["datasetTitle"].get<std::string>() << "</gml:name>" << std::endl;
    }
  }
}


void surface(std::vector<std::vector<int>> &onesurface) {
  std::cout << "<gml:surfaceMember>" << std::endl;
  std::cout << "<gml:Polygon>" << std::endl;
  std::cout << "<gml:exterior>" << std::endl;
  std::cout << "<gml:LinearRing>" << std::endl;
  for (auto& v: onesurface[0]) {
    std::cout << "<gml:pos>";
    std::cout << j["vertices"][v][0] << " ";
    std::cout << j["vertices"][v][1] << " ";
    std::cout << j["vertices"][v][2];
    std::cout << "</gml:pos>" << std::endl;
  }
  //-- repeat first vertex
  auto firstv = onesurface[0][0];
  std::cout << "<gml:pos>";
  std::cout << j["vertices"][firstv][0] << " ";
  std::cout << j["vertices"][firstv][1] << " ";
  std::cout << j["vertices"][firstv][2];
  std::cout << "</gml:pos>" << std::endl;
  std::cout << "</gml:LinearRing>" << std::endl;
  std::cout << "</gml:exterior>" << std::endl;
  if (onesurface.size() > 1) {
    for (int i = 1; i < onesurface.size(); i++) {
      std::cout << "<gml:interior>" << std::endl;
      std::cout << "<gml:LinearRing>" << std::endl;
      for (auto& v: onesurface[i]) {
        std::cout << "<gml:pos>";
        std::cout << j["vertices"][v][0] << " ";
        std::cout << j["vertices"][v][1] << " ";
        std::cout << j["vertices"][v][2];
        std::cout << "</gml:pos>" << std::endl;
      }
      //-- repeat first vertex
      auto firstv = onesurface[i][0];
      std::cout << "<gml:pos>";
      std::cout << j["vertices"][firstv][0] << " ";
      std::cout << j["vertices"][firstv][1] << " ";
      std::cout << j["vertices"][firstv][2];
      std::cout << "</gml:pos>" << std::endl;
      std::cout << "</gml:LinearRing>" << std::endl;
      std::cout << "</gml:interior>" << std::endl;
    }
  }
  std::cout << "</gml:Polygon>" << std::endl;
  std::cout << "</gml:surfaceMember>" << std::endl;
}


void shell(json& jsh) {
  std::cout << "<gml:CompositeSurface>" << std::endl;
  for (auto& polygon : jsh) { 
    std::vector<std::vector<int>> t = polygon;
    surface(t);
  }
  std::cout << "</gml:CompositeSurface>" << std::endl;
}


void solid(json& js) {
  std::cout << "<gml:Solid>" << std::endl;
  int numshell = 0;
  for (auto& sh : js["boundaries"]) {
    if (numshell == 0) 
      std::cout << "<gml:exterior>" << std::endl;
    else
      std::cout << "<gml:interior>" << std::endl;
    shell(sh);
    if (numshell == 0) 
      std::cout << "</gml:exterior>" << std::endl;
    else
      std::cout << "</gml:interior>" << std::endl;
    numshell++;
  }
  std::cout << "</gml:Solid>" << std::endl;
}


void solid_w_semantics(json& js) {
  int i = 0;
  int lod = js["lod"].get<int>();
  boost::uuids::uuid myuuid = boost::uuids::random_generator()();
  for (auto& semsur : js["semantics"]["surfaces"]) {
    std::cout << "<bldg:boundedBy>" << std::endl;
    std::cout << "<bldg:" << semsur["type"].get<std::string>();
    std::cout << " gml:id=\"" << myuuid << "_" << i << "\">" << std::endl;
    //-- extra attributes
    for (json::iterator it = semsur.begin(); it != semsur.end(); ++it) {
      if (it.key() != "type") {
        std::cout << "<" << it.key() << ">";
        std::cout << it.value();
        std::cout << "</" << it.key() << ">" << std::endl;
      }
    }
    //-- geometry
    std::cout << "<bldg:lod" << lod << "MultiSurface>" << std::endl;
    std::cout << "<gml:MultiSurface>" << std::endl;

    int pi = 0, pj = 0;
    for (auto& shell : js["semantics"]["values"]) { 
      pj = 0;
      for (auto& sur : shell) { 
        if (sur == i) {
          std::vector<std::vector<int>> t = js["boundaries"][pi][pj];
          surface(t);
        }
        pj++;
      }
      pi++;
    }
    std::cout << "</gml:MultiSurface>" << std::endl;
    std::cout << "</bldg:lod" << lod << "MultiSurface>" << std::endl;
    std::cout << "</bldg:" << semsur["type"].get<std::string>() << ">" << std::endl;
    std::cout << "</bldg:boundedBy>" << std::endl;
    i++;
  }
  //-- bundle the sem-surfaces with a GML geometry
  std::cout << "<bldg:lod" << lod << js["type"].get<std::string>() << ">" << std::endl;
  std::cout << "<gml:Solid>" << std::endl;
  int pi = 0, pj = 0;
  for (auto& shell : js["semantics"]["values"]) { 
    if (pi == 0) 
      std::cout << "<gml:exterior>" << std::endl;
    else
      std::cout << "<gml:interior>" << std::endl;
    std::set<int> aset;
    bool anynull = false;
    for (auto& each : shell) {
      if (each.is_null() == false)
        aset.insert(each.get<int>());
      else
        anynull = true;
    }
    for (auto& each : aset) { 
      std::cout << "<gml:surfaceMember xlink:href=\"";
      std::cout << "#" << myuuid << "_" << each << "\"/>" << std::endl;
    }
    if (anynull == true) {
      pj = 0;
      for (auto& each : shell) {
        if (each.is_null() == true) {
          std::vector<std::vector<int>> t = js["boundaries"][pi][pj];
          surface(t);
        }
        pj++;
      }
    }
    if (pi == 0) 
      std::cout << "</gml:exterior>" << std::endl;
    else
      std::cout << "</gml:interior>" << std::endl;
    pi++;
  }
  std::cout << "</gml:Solid>" << std::endl;
  std::cout << "</bldg:lod" << lod << js["type"].get<std::string>() << ">" << std::endl;
}


void multisurface_w_semantics(json& js) {
  int i = 0;
  int lod = js["lod"].get<int>();
  boost::uuids::uuid myuuid = boost::uuids::random_generator()();
  for (auto& semsur : js["semantics"]["surfaces"]) {
    std::cout << "<bldg:boundedBy>" << std::endl;
    std::cout << "<bldg:" << semsur["type"].get<std::string>();
    std::cout << " gml:id=\"" << myuuid << "_" << i << "\">" << std::endl;
    //-- extra attributes
    for (json::iterator it = semsur.begin(); it != semsur.end(); ++it) {
      if (it.key() != "type") {
        std::cout << "<" << it.key() << ">";
        std::cout << it.value();
        std::cout << "</" << it.key() << ">" << std::endl;
      }
    }
    //-- geometry
    std::cout << "<bldg:lod" << lod << "MultiSurface>" << std::endl;
    std::cout << "<gml:MultiSurface>" << std::endl;
    int pi = 0;
    for (auto& sur : js["semantics"]["values"]) { 
      if (sur == i) {
        std::vector<std::vector<int>> t = js["boundaries"][pi];
        surface(t);
      }
      pi++;
    }
    std::cout << "</gml:MultiSurface>" << std::endl;
    std::cout << "</bldg:lod" << lod << "MultiSurface>" << std::endl;
    std::cout << "</bldg:" << semsur["type"].get<std::string>() << ">" << std::endl;
    std::cout << "</bldg:boundedBy>" << std::endl;
    i++;
  }
  //-- bundle the sem-surfaces with a GML geometry
  std::cout << "<bldg:lod" << lod << js["type"].get<std::string>() << ">" << std::endl;
  if (js["type"] == "MultiSurface") {
    std::cout << "<gml:MultiSurface>" << std::endl;
    std::set<int> aset;
    bool anynull = false;
    for (auto& sems : js["semantics"]["values"]) {
      if (sems.is_null() == false)
        aset.insert(sems.get<int>());
      else
        anynull = true;
    }
    for (auto& each : aset) { 
      std::cout << "<gml:surfaceMember xlink:href=\"";
      std::cout << "#" << myuuid << "_" << each << "\"/>" << std::endl;
    }
    if (anynull == true) {
      int k = 0;
      for (auto& each : js["semantics"]["values"]) {
        if (each.is_null() == true) {
          std::vector<std::vector<int>> t = js["boundaries"][k];
          surface(t);
        }
        k++;
      }
    }
    std::cout << "</gml:MultiSurface>" << std::endl;
  }
  std::cout << "</bldg:lod" << lod << js["type"].get<std::string>() << ">" << std::endl;
}


void multisurface(json& js) {
  std::cout << "<gml:MultiSurface>" << std::endl;
  for (auto& polygon : js["boundaries"]) {
    std::vector<std::vector<int>> t = polygon;
    surface(t);
  }
  std::cout << "</gml:MultiSurface>" << std::endl;
}


void compositesurface(json& js) {
  std::cout << "<gml:CompositeSurface>" << std::endl;
  for (auto& polygon : js["boundaries"]) {
    std::vector<std::vector<int>> t = polygon;
    surface(t);
  }
  std::cout << "</gml:CompositeSurface>" << std::endl;
}


void attributes(json& jco) {
  json tmp;
  std::string cotype = jco["type"];
  tmp = jschema["definitions"][cotype]["properties"]["attributes"]["properties"];
  for (json::iterator it = jco["attributes"].begin(); it != jco["attributes"].end(); ++it) {
    if (tmp.find(it.key()) != tmp.end()) {
      std::cout << "<" << it.key() << ">";
      if (it.value().is_string())
        std::cout << it.value().get<std::string>();
      else
        std::cout << it.value();
      std::cout << "</" << it.key() << ">" << std::endl;
    }
    else {
      // std::cout << "ATTRIBUTE GENERICS" << std::endl;
      std::cout << "<gen:stringAttribute name=\"" << it.key() << "\">" << std::endl;
      if (it.value().is_string())
        std::cout << "<gen:value>" << it.value().get<std::string>() << "</gen:value>" << std::endl;
      else
        std::cout << "<gen:value>" << it.value() << "</gen:value>" << std::endl;
      std::cout << "</gen:stringAttribute>" << std::endl;
    }
  }
}


void building(std::string id, json& jb) {
  std::cout << "<bldg:Building gml:id=\"" << id << "\">" << std::endl;
  //-- 1. attributes
  attributes(jb);
  // 2. each geoms
  for (auto& g : jb["geometry"]) {
    if (g.count("semantics") != 0) {
      if (g["type"] == "Solid")
        solid_w_semantics(g);
      if (g["type"] == "MultiSurface")
        multisurface_w_semantics(g);
    }
    else {
      int lod = g["lod"].get<int>();
      std::cout << "<bldg:lod" << lod << g["type"].get<std::string>() << ">" << std::endl;
      if (g["type"] == "Solid")
        solid(g);
      if (g["type"] == "MultiSurface")
        multisurface(g);
      std::cout << "</bldg:lod" << lod << g["type"].get<std::string>() << ">" << std::endl;
    }
  }

  // 3. TODO: BuildingParts?
  // 4. TODO: BuildingInstallations?

  std::cout << "</bldg:Building>" << std::endl;
}


void geometry(json& jg, bool csasms) {
  if (jg["type"] == "Solid")
    solid(jg);
  if (jg["type"] == "MultiSurface")
    multisurface(jg);
  if (jg["type"] == "CompositeSurface") {
    if (csasms == true)   
      multisurface(jg);
    else
      compositesurface(jg);
  }
}


void waterbody(std::string id, json& jco) {
  std::cout << "<wtr:WaterBody gml:id=\"" << id << "\">" << std::endl;
  //-- 1. attributes
  attributes(jco);
  //-- 2. geometry
  for (auto& g : jco["geometry"]) {
    if (g.count("semantics") != 0) {
      // std::cout << "TODO" << std::endl;
    }
    else {
      int lod = g["lod"].get<int>();
      std::stringstream ss;
      ss << "<wtr:lod" << lod;
      std::string cotype = g["type"].get<std::string>();
      bool csasms = false;
      if (g["type"] == "CompositeSurface") {
        cotype = "MultiSurface";
        csasms = true;
      }
      ss << cotype << ">";
      std::cout << ss.str() << std::endl;
      geometry(g, csasms);
      std::string tmp = ss.str();
      tmp = tmp.insert(1, "/");
      std::cout << tmp;
    }
  }
  std::cout << "</wtr:WaterBody>" << std::endl;
}


void landuse(std::string id, json& jco) {
  std::cout << "<luse:LandUse gml:id=\"" << id << "\">" << std::endl;
  //-- 1. attributes
  attributes(jco);
  //-- 2. geometry
  for (auto& g : jco["geometry"]) {
    if (g.count("semantics") != 0) {
      // std::cout << "TODO" << std::endl;
    }
    else {
      int lod = g["lod"].get<int>();
      std::stringstream ss;
      ss << "<luse:lod" << lod;
      std::string cotype = g["type"].get<std::string>();
      bool csasms = false;
      if (g["type"] == "CompositeSurface") {
        cotype = "MultiSurface";
        csasms = true;
      }
      ss << cotype << ">";
      std::cout << ss.str() << std::endl;
      geometry(g, csasms);
      std::string tmp = ss.str();
      tmp = tmp.insert(1, "/");
      std::cout << tmp;
    }
  }
  std::cout << "</luse:LandUse>" << std::endl;
}


void cityobject(std::string id, json& jco) {
  std::cout << "<cityObjectMember>" << std::endl;
  if (jco["type"] == "Building")
    building(id, jco);
  else if (jco["type"] == "WaterBody")
    waterbody(id, jco);
  else
    std::cerr << jco["type"] <<  ": NOT IMPLEMENTED YET." << std::endl;
  std::cout << "</cityObjectMember>" << std::endl;
}


int main(int argc, const char * argv[]) {
  //-- the input file
  const char* filename = argv[1];
  std::ifstream input(filename);
  input >> j;
  //-- the schema
  std::string v = j["version"];
  if (v.find("http://www.cityjson.org/version/") != std::string::npos)
      v = v.substr(v.size() - 3);
  std::string nameschema = "../../../../schema/cityjson-v" + v.substr(0, 1) + v.substr(2) + ".schema.json";
  std::ifstream inputs(nameschema);
  // std::cout << "Schema: " << nameschema << std::endl << std::endl;
  if (inputs.is_open() == true) {
    inputs >> jschema;
  }
  else {
    std::cout << "ERROR:   cannot find schema v" << v << std::endl << std::endl;  
    return 0;
  }

  std::cout << std::setprecision(3) << std::fixed;
  xml_header();
  std::cout << "<!-- Automatically converted from CityJSON (http://www.cityjson.org) -->\n";
  citygml_header();
  metadata();
  
  for (json::iterator coit = j["CityObjects"].begin(); coit != j["CityObjects"].end(); ++coit)
      cityobject(coit.key(), coit.value());

  std::cout << "</CityModel>" << std::endl;
  return 0;
}