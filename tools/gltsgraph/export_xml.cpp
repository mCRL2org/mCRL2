#include "export_xml.h"
#include "ticpp.h"

ExporterXML::ExporterXML(Graph* g) : Exporter(g)
{

};

ExporterXML::~ExporterXML(){};

bool ExporterXML::export_to(wxString _filename)
{
  try
  {

    ticpp::Document xmlDoc(_filename.fn_str());
  
    ticpp::Declaration* decl = new ticpp::Declaration("1.0", "", "");
    xmlDoc.LinkEndChild(decl);
    

    // Add a graph element to the document
    ticpp::Element* graphEl = new ticpp::Element("Graph");
    graphEl->SetAttribute("name", "Foo"); // TODO: Graph names
    xmlDoc.LinkEndChild(graphEl);
    
    // A Graph has two types of children, states and transitions.
    for(size_t i = 0; i < graph->getNumberOfStates(); ++i)
    {
      State* s = graph->getState(i);
      ticpp::Element* state = new ticpp::Element("State");

      size_t fromVal = s->getValue();

      state->SetAttribute("value", fromVal);
      state->SetAttribute("isInitial", s->isInitialState());
      state->SetAttribute("x", s->getX());
      state->SetAttribute("y", s->getY());
      
      graphEl->LinkEndChild(state);
      
      for(size_t j = 0; j < s->getNumberOfTransitions(); ++j)
      {
        Transition* t = s->getTransition(j);
        ticpp::Element* transition = new ticpp::Element("Transition");
        
        size_t toVal = t->getTo()->getValue();

        transition->SetAttribute("from", fromVal);
        transition->SetAttribute("to", toVal);
        transition->SetAttribute("label", t->getLabel());
        
        double x, y;
        t->getControl(x, y);
        transition->SetAttribute("x", x);
        transition->SetAttribute("y", y);

        graphEl->LinkEndChild(transition);
      }

      for(size_t j = 0; j < s->getNumberOfSelfLoops(); ++j)
      {
        Transition* t = s->getSelfLoop(j);
        ticpp::Element* transition = new ticpp::Element("Transition");

        transition->SetAttribute("from", fromVal);
        transition->SetAttribute("to", fromVal);
        transition->SetAttribute("label", t->getLabel());

        double x, y;
        t->getControl(x, y);
        transition->SetAttribute("x", x);
        transition->SetAttribute("y", y);

        graphEl->LinkEndChild(transition);
      }
    }
    
    xmlDoc.SaveFile();

    return true;
  }
  catch(ticpp::Exception e)
  {
    std::cerr << "Exception by ticpp: " << e.what();
    return false;
  }

}

