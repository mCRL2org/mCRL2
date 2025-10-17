#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <tuple>
#include <algorithm>

using namespace std;

// Simplified structures for testing
struct SimpleXmlNode {
    std::string name;
    std::string content;
    std::unordered_map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<SimpleXmlNode>> children;
    
    SimpleXmlNode* find_child(const std::string& child_name) {
        for (auto& child : children) {
            if (child->name == child_name) {
                return child.get();
            }
        }
        return nullptr;
    }
    
    std::string get_attribute(const std::string& attr_name) {
        auto it = attributes.find(attr_name);
        return (it != attributes.end()) ? it->second : "";
    }
};

// Simple XML parser class
class SimpleXmlParser {
private:
    static std::unique_ptr<SimpleXmlNode> parse_node(const std::string& xml_content, size_t& pos) {
        // Skip whitespace and comments
        while (pos < xml_content.length()) {
            // Skip whitespace
            while (pos < xml_content.length() && std::isspace(xml_content[pos])) {
                pos++;
            }
            
            if (pos >= xml_content.length()) {
                return nullptr;
            }
            
            // Check for comment
            if (pos < xml_content.length() - 4 && xml_content.substr(pos, 4) == "<!--") {
                // Skip comment
                size_t comment_end = xml_content.find("-->", pos + 4);
                if (comment_end != std::string::npos) {
                    pos = comment_end + 3;
                    continue;
                } else {
                    return nullptr;
                }
            }
            
            // Check for regular element
            if (xml_content[pos] == '<') {
                break;
            }
            
            pos++;
        }
        
        if (pos >= xml_content.length() || xml_content[pos] != '<') {
            return nullptr;
        }
        
        auto node = std::make_unique<SimpleXmlNode>();
        
        // Find end of opening tag
        size_t tag_end = xml_content.find('>', pos);
        if (tag_end == std::string::npos) {
            return nullptr;
        }
        
        std::string tag = xml_content.substr(pos + 1, tag_end - pos - 1);
        
        // Check for self-closing tag
        bool self_closing = (tag.back() == '/');
        if (self_closing) {
            tag.pop_back();
        }
        
        // Extract tag name and attributes
        std::istringstream tag_stream(tag);
        tag_stream >> node->name;
        
        // Parse attributes
        std::string attr_part;
        std::getline(tag_stream, attr_part);
        std::regex attr_regex(R"((\w+)=\"([^\"]*)\")");
        std::sregex_iterator attr_iter(attr_part.begin(), attr_part.end(), attr_regex);
        std::sregex_iterator attr_end;
        
        for (; attr_iter != attr_end; ++attr_iter) {
            node->attributes[(*attr_iter)[1].str()] = (*attr_iter)[2].str();
        }
        
        pos = tag_end + 1;
        
        if (self_closing) {
            return node;
        }
        
        // Parse content and children
        size_t content_start = pos;
        std::string closing_tag = "</" + node->name + ">";
        
        while (pos < xml_content.length()) {
            if (xml_content.substr(pos, closing_tag.length()) == closing_tag) {
                // Found closing tag
                std::string content = xml_content.substr(content_start, pos - content_start);
                
                // Check if content contains only text (no child elements)
                if (content.find('<') == std::string::npos) {
                    // Trim whitespace
                    content.erase(0, content.find_first_not_of(" \t\n\r"));
                    content.erase(content.find_last_not_of(" \t\n\r") + 1);
                    node->content = content;
                } else {
                    // Parse child elements
                    size_t child_pos = content_start;
                    while (child_pos < pos) {
                        if (xml_content[child_pos] == '<' && xml_content[child_pos + 1] != '/') {
                            auto child = parse_node(xml_content, child_pos);
                            if (child) {
                                node->children.push_back(std::move(child));
                            }
                        } else {
                            child_pos++;
                        }
                    }
                }
                
                pos += closing_tag.length();
                return node;
            }
            pos++;
        }
        
        return node;
    }

public:
    static std::unique_ptr<SimpleXmlNode> parse_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return nullptr;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        return parse_string(content);
    }
    
    static std::unique_ptr<SimpleXmlNode> parse_string(const std::string& xml_content) {
        size_t pos = 0;
        // Skip XML declaration if present
        if (xml_content.substr(0, 5) == "<?xml") {
            pos = xml_content.find("?>", 0);
            if (pos != std::string::npos) {
                pos += 2;
            } else {
                pos = 0;
            }
        }
        
        return parse_node(xml_content, pos);
    }
};

static std::string pn2gsGetText(SimpleXmlNode* cur) {
    if (!cur) return "";
    
    for (auto& child : cur->children) {
        if (child->name == "text") {
            return child->content;
        }
    }
    return "";
}

// Simple context structure
struct Context {
    std::unordered_map<std::string, std::string> place_name;
    std::unordered_map<std::string, std::string> place_mark;
    std::unordered_map<std::string, std::string> trans_name;
    std::unordered_map<std::string, std::pair<std::string, std::string>> arc_in;   // arc_id -> (trans_id, place_id)
    std::unordered_map<std::string, std::pair<std::string, std::string>> arc_out;  // arc_id -> (trans_id, place_id)
    bool Abort = false;
};

static Context context;

bool do_pnml2mcrl2_test(const char* InFileName, std::ostream& output_stream) {
    try {
        // Initialize context
        context.Abort = false;
        context.place_name.clear();
        context.place_mark.clear();
        context.trans_name.clear();
        context.arc_in.clear();
        context.arc_out.clear();
        
        // Parse XML file using our simple parser
        auto doc = SimpleXmlParser::parse_file(InFileName);
        if (!doc) {
            std::cerr << "Error: Could not parse PNML file: " << InFileName << std::endl;
            return false;
        }

        // Check if it's a PNML file
        if (doc->name != "pnml") {
            std::cerr << "Error: File is not a usable PNML file!" << std::endl;
            return false;
        }

        // Find first <net> element
        SimpleXmlNode* net_node = nullptr;
        for (auto& child : doc->children) {
            if (child->name == "net") {
                net_node = child.get();
                break;
            }
        }

        if (!net_node) {
            std::cerr << "Error: No <net> element found" << std::endl;
            return false;
        }

        // Get net ID
        std::string net_id = net_node->get_attribute("id");
        if (net_id.empty()) {
            net_id = "default_net";
        }

        // Store arc data for later processing
        std::vector<std::tuple<std::string, std::string, std::string>> arc_data;

        // First pass: Process places and transitions only
        for (auto& child : net_node->children) {
            if (child->name == "place") {
                // Process place
                std::string place_id = child->get_attribute("id");
                if (place_id.empty()) {
                    context.Abort = true;
                    return false;
                }

                std::string name = "default_name";
                std::string initial_marking = "0";

                // Process place children for name and initial marking
                for (auto& place_child : child->children) {
                    if (place_child->name == "name") {
                        std::string name_text = pn2gsGetText(place_child.get());
                        if (!name_text.empty()) {
                            name = name_text;
                        }
                    } else if (place_child->name == "initialMarking") {
                        std::string marking_text = pn2gsGetText(place_child.get());
                        if (!marking_text.empty()) {
                            initial_marking = marking_text;
                        }
                    }
                }

                // Store in context tables
                context.place_name[place_id] = name;
                context.place_mark[place_id] = initial_marking;

            } else if (child->name == "transition") {
                // Process transition
                std::string trans_id = child->get_attribute("id");
                if (trans_id.empty()) {
                    context.Abort = true;
                    return false;
                }

                std::string name = "default_name";

                // Process transition children for name
                for (auto& trans_child : child->children) {
                    if (trans_child->name == "name") {
                        std::string name_text = pn2gsGetText(trans_child.get());
                        if (!name_text.empty()) {
                            name = name_text;
                        }
                    }
                }

                // Store in context tables
                context.trans_name[trans_id] = name;

            } else if (child->name == "arc") {
                // Store arc data for later processing
                std::string arc_id = child->get_attribute("id");
                std::string source = child->get_attribute("source");
                std::string target = child->get_attribute("target");
                
                if (!arc_id.empty() && !source.empty() && !target.empty()) {
                    arc_data.emplace_back(arc_id, source, target);
                }
            }
        }

        // Second pass: Process arcs now that we know all places and transitions
        for (const auto& [arc_id, source, target] : arc_data) {
            // Determine arc type based on source/target being place or transition
            bool source_is_place = (context.place_name.find(source) != context.place_name.end());
            bool target_is_place = (context.place_name.find(target) != context.place_name.end());
            bool source_is_trans = (context.trans_name.find(source) != context.trans_name.end());
            bool target_is_trans = (context.trans_name.find(target) != context.trans_name.end());

            if (source_is_place && target_is_trans) {
                // Arc from place to transition (input arc for transition)
                context.arc_in[arc_id] = std::make_pair(target, source);
            } else if (source_is_trans && target_is_place) {
                // Arc from transition to place (output arc for transition)
                context.arc_out[arc_id] = std::make_pair(source, target);
            }
        }

        // Generate improved mCRL2 specification with proper Petri net semantics
        output_stream << "% Generated from PNML file: " << InFileName << std::endl;
        output_stream << "% Improved implementation with proper Petri net semantics" << std::endl;
        output_stream << std::endl;
        
        // Generate mCRL2 structure with proper Petri net semantics
        output_stream << "map" << std::endl;
        output_stream << "  nMaxTokens: Nat;" << std::endl;
        output_stream << std::endl;
        
        output_stream << "eqn" << std::endl;
        output_stream << "  nMaxTokens = 5;" << std::endl; // 5 tokens max
        output_stream << std::endl;
        
        output_stream << "act" << std::endl;
        
        // Generate actions for place/transition synchronization
        // First detect self-loops (transitions that both consume and produce from same place)
        std::unordered_set<std::string> self_loop_transitions;
        
        // Build maps: trans_id -> set of input places and output places
        std::unordered_map<std::string, std::unordered_set<std::string>> trans_input_places;
        std::unordered_map<std::string, std::unordered_set<std::string>> trans_output_places;
        
        for (const auto& [arc_id, arc_data] : context.arc_in) {
            const std::string& trans_id = arc_data.first;
            const std::string& place_id = arc_data.second;
            trans_input_places[trans_id].insert(place_id);
        }
        
        for (const auto& [arc_id, arc_data] : context.arc_out) {
            const std::string& trans_id = arc_data.first;
            const std::string& place_id = arc_data.second;
            trans_output_places[trans_id].insert(place_id);
        }
        
        // Check for self-loops: transitions that have common input and output places
        for (const auto& [trans_id, _] : context.trans_name) {
            auto input_it = trans_input_places.find(trans_id);
            auto output_it = trans_output_places.find(trans_id);
            
            if (input_it != trans_input_places.end() && output_it != trans_output_places.end()) {
                // Check for intersection of input and output places
                for (const std::string& input_place : input_it->second) {
                    if (output_it->second.count(input_place)) {
                        self_loop_transitions.insert(trans_id);
                        break;
                    }
                }
            }
        }
        
        bool first_action = true;
        
        // For each transition, generate appropriate actions
        for (const auto& [trans_id, trans_name_val] : context.trans_name) {
            if (!first_action) output_stream << ";" << std::endl;
            
            if (self_loop_transitions.count(trans_id)) {
                // Self-loop: use single atomic action
                output_stream << "  t_" << trans_id;
            } else {
                // Regular transition: use input/output synchronization
                output_stream << "  t_" << trans_id << "_in, t_" << trans_id << "_out, t_" << trans_id;
            }
            first_action = false;
        }
        
        output_stream << ";" << std::endl << std::endl;
        
        output_stream << "proc" << std::endl;
        
        // Generate place processes with proper token management
        for (const auto& [place_id, place_name_val] : context.place_name) {
            // Find input and output arcs for this place
            std::vector<std::string> input_transitions;  // transitions that output to this place
            std::vector<std::string> output_transitions; // transitions that take from this place
            
            // Check all arcs to find connections to this place
            for (const auto& [arc_id, trans_place_pair] : context.arc_out) {
                // arc_out contains transition->place arcs
                if (trans_place_pair.second == place_id) {
                    input_transitions.push_back(trans_place_pair.first);
                }
            }
            
            for (const auto& [arc_id, trans_place_pair] : context.arc_in) {
                // arc_in contains transition<-place arcs (stored as transition,place)
                if (trans_place_pair.second == place_id) {
                    output_transitions.push_back(trans_place_pair.first);
                }
            }
            
            output_stream << "  P_" << place_id << "(n: Nat) = ";
            
            bool has_alternatives = false;
            
            // Add alternatives for self-loop transitions involving this place
            for (const std::string& trans_id : self_loop_transitions) {
                // Check if this place is involved in this self-loop transition
                bool place_in_self_loop = false;
                auto input_it = trans_input_places.find(trans_id);
                if (input_it != trans_input_places.end() && input_it->second.count(place_id)) {
                    auto output_it = trans_output_places.find(trans_id);
                    if (output_it != trans_output_places.end() && output_it->second.count(place_id)) {
                        place_in_self_loop = true;
                    }
                }
                
                if (place_in_self_loop) {
                    if (has_alternatives) output_stream << " + ";
                    output_stream << "(n > 0) -> t_" << trans_id << " . P_" << place_id << "(n)";
                    has_alternatives = true;
                }
            }
            
            // Add alternatives for each input transition (transitions that can add tokens)
            // Exclude self-loop transitions as they're handled separately
            for (const std::string& trans : input_transitions) {
                if (self_loop_transitions.count(trans) == 0) {
                    if (has_alternatives) output_stream << " + ";
                    output_stream << "t_" << trans << "_out . P_" << place_id << "(n + 1)";
                    has_alternatives = true;
                }
            }
            
            // Add alternatives for each output transition (transitions that can remove tokens)
            // Exclude self-loop transitions as they're handled separately
            for (const std::string& trans : output_transitions) {
                if (self_loop_transitions.count(trans) == 0) {
                    if (has_alternatives) output_stream << " + ";
                    output_stream << "(n > 0) -> t_" << trans << "_in . P_" << place_id << "(Int2Nat(n - 1))";
                    has_alternatives = true;
                }
            }
            
            if (!has_alternatives) {
                // Place with no connections - deadlock
                output_stream << "delta";
            }
            
            output_stream << ";" << std::endl;
        }
        
        // Generate transition processes - transitions don't need separate processes 
        // since they synchronize directly with places
        output_stream << std::endl;
        output_stream << "init" << std::endl;
        output_stream << "  allow({";
        
        // Generate allow set for all transition actions
        first_action = true;
        for (const auto& [trans_id, _] : context.trans_name) {
            if (!first_action) output_stream << ", ";
            output_stream << "t_" << trans_id;
            first_action = false;
        }
        
        output_stream << "}," << std::endl << "    comm({";
        
        // Generate communication set for input/output synchronization
        // Exclude self-loop transitions as they use single atomic actions
        first_action = true;
        for (const auto& [trans_id, _] : context.trans_name) {
            if (self_loop_transitions.count(trans_id) == 0) {
                if (!first_action) output_stream << ", ";
                output_stream << "t_" << trans_id << "_out|t_" << trans_id << "_in -> t_" << trans_id;
                first_action = false;
            }
        }
        
        output_stream << "}," << std::endl << "        ";
        
        // Generate initial process composition - just the places
        first_action = true;
        
        // Add all place processes with their initial markings
        for (const auto& [place_id, _] : context.place_name) {
            auto marking_it = context.place_mark.find(place_id);
            std::string initial_tokens = "0"; // Default to 0 tokens
            if (marking_it != context.place_mark.end()) {
                initial_tokens = marking_it->second;
            }
            
            if (!first_action) output_stream << " || ";
            output_stream << "P_" << place_id << "(" << initial_tokens << ")";
            first_action = false;
        }
        
        output_stream << std::endl << "    )" << std::endl << "  );" << std::endl;

        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during conversion: " << e.what() << std::endl;
        return false;
    }
}

int main(int argc, char** argv) {
    const char* filename = (argc > 1) ? argv[1] : "test.pnml";
    if (do_pnml2mcrl2_test(filename, cout)) {
        return 0;
    } else {
        return 1;
    }
}
