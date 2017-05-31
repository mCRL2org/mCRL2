// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file block_tree.h


#ifndef MCRL2_LPSSYMBOLICBISIM_BLOCK_TREE_H
#define MCRL2_LPSSYMBOLICBISIM_BLOCK_TREE_H



namespace mcrl2
{
namespace data
{

class block_tree
{
protected:
  const data_expression self;
  std::list< block_tree* > children;
  bool is_deleted = false;

  void output_aux(std::ofstream& outstream, const std::set< data_expression >& current_partition, int& node_count)
  {
    int own_label = node_count;
    node_count++;
    std::string color;
    if(is_deleted)
    {
      color = "red";
    }
    else if(current_partition.find(self) != current_partition.end())
    {
      color = "blue";
    }
    else
    {
      color = "black";
    }
    outstream << own_label << " [label=\"" << self << "\"" << ",color=" << color << ",fontcolor=" << color << "];" << std::endl;
    for(block_tree* child: children)
    {
      int child_label = node_count;
      child->output_aux(outstream, current_partition, node_count);
      outstream << own_label << " -> " << child_label << ";" << std::endl;
    }

  }

public:
  block_tree(const data_expression& self_)
  : self(self_)
  {
  }

  ~block_tree()
  {
    for(block_tree* child: children)
    {
      delete child;
    }
  }

  template <typename Container>
  void add_children(Container new_children, typename atermpp::enable_if_container<Container, data_expression>::type* = nullptr)
  {
    for(const data_expression& block: new_children)
    {
      add_child(block);
    }
  }

  void add_child(const data_expression& new_child)
  {
    block_tree* node = new block_tree(new_child);
    children.push_back(node);
  }

  block_tree* find(const data_expression& block)
  {
    if(self == block)
    {
      return this;
    }
    
    for(block_tree* child: children)
    {
      block_tree* sub_result = child->find(block);
      if(sub_result != nullptr)
      {
        return sub_result;
      }
    }

    return nullptr;
  }

  void mark_deleted(const data_expression& block)
  {
    if(block == self)
    {
      is_deleted = true;
    }
    for(block_tree* child: children)
    {
      child->mark_deleted(block);
    }
  }

  void output_dot(const std::string& filename, const std::set< data_expression >& current_partition)
  {
    std::ofstream outstream;
    outstream.open(filename);
    outstream << "digraph split_tree {" << std::endl;
    int node_count = 0;
    output_aux(outstream, current_partition, node_count);
    outstream << "}" << std::endl;
    outstream.close();
  }
};

} // namespace data
} // namespace mcrl2


#endif // MCRL2_LPSSYMBOLICBISIM_BLOCK_TREE_H
