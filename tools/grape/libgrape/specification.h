// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file specification.h
//
// Declares the grape_specification class.

#ifndef LIBGRAPE_GRAPESPECIFICATION_H
#define LIBGRAPE_GRAPESPECIFICATION_H

#include "datatypespecification.h"
#include "processdiagram.h"
#include "architecturediagram.h"

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a GraPE specification.
     * @see diagram
     * @see architecture_diagram
     * @see process_diagram
     */
    class grape_specification
    {
      protected:
        datatype_specification m_datatype_specification;  /**< datatype specifications of this specification. */

        arr_process_diagram       m_process_diagrams; /**< process diagrams contained in this specification. */
        arr_architecture_diagram  m_architecture_diagrams;       /**< architecture diagrams contained in this specification. */

      public:
        /**
         * Default constructor.
         * Initializes grape_specification.
         */
        grape_specification( void );

        /**
         * Copy constructor.
         * Creates a new GraPE specification based on an existing one.
         * @param p_spec The specification to copy.
         */
        grape_specification( const grape_specification &p_spec );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_specification( void );

        /**
         * @return A pointer to the datatype specification
         */

         datatype_specification* get_datatype_specification( void );

         /**
          * To be used exclusively during XML-conversion.
          * @return A pointer to list of process diagrams
          */
         arr_process_diagram* get_process_diagram_list( void );

        /**
          * To be used exclusively during XML-conversion.
          * @return A pointer to list of architecture diagrams
          */
         arr_architecture_diagram* get_architecture_diagram_list( void );

        /**
         * Architecture diagram addition function.
         * Creates a new architecture diagram. If an architecture diagram with the specified name already exists, an index is appended to the name.
         * @param p_id The identifier of the new architecture diagram.
         * @param p_name The name of the new architecture diagram.
         * @param p_index The index used to eventually make a new unique name if the current suggested name exists already.
         * @return Returns a pointer to the newly created architecture diagram.
         */
        architecture_diagram* add_architecture_diagram( unsigned int p_id, const wxString &p_name, int p_index = 0 );

        /**
         * Architecture diagram removal function.
         * Removes the architecture diagram and all objects in it.
         * @param p_arch_dia The architecture diagram.
         */
        void remove_architecture_diagram( architecture_diagram* p_arch_dia );

        /**
         * Architecture diagram count function.
         * @return Returns the number of architecture diagrams in the specification.
         */
        unsigned int count_architecture_diagram( void );

        /**
         * Architecture diagram retrieval function.
         * @return Returns the architecture diagram at position p_index in the list of architecture diagrams.
         */
        architecture_diagram* get_architecture_diagram( int p_index );

          /**
         * Process diagram addition function.
         * Creates a new process diagram.
         * @param p_id The identifier of the new process diagram.
         * @param p_name The name of the new process diagram.
         * @param p_index The index used to eventually make a new unique name if the current suggested name exists already.
         * @return Returns a pointer to the newly created process diagram.
         */
        process_diagram* add_process_diagram( unsigned int p_id, const wxString &p_name, int p_index = 0 );

        /**
         * Process diagram removal function.
         * Removes the process diagram and all objects in it.
         * @param p_proc_dia The process diagram.
         */
        void remove_process_diagram( process_diagram* p_proc_dia );

        /**
         * Process diagram count function.
         * @return Returns the number of process diagrams in the specification.
         */
        unsigned int count_process_diagram( void );

        /**
         * Process diagram retrieval function.
         * @return Returns the process diagram at position p_index in the list of process diagrams.
         */
        process_diagram* get_process_diagram( int p_index );

        /**
         * Object retrieval function through all diagrams in the specification.
         * @param p_id The identifier of the desired object.
         * @param p_type The type of the object you want to retrieve.
         * @return Returns the object that has the specified id, if it exists (0 if not).
         */
        static object *find_object( grape_specification *p_spec, unsigned int p_id, object_type p_type = ANY );

        /**
         * Architecture reference check function.
         * Given the specified name, finds all references that point to diagrams of the specified type and makes it point to the specified diagram pointer.
         * @param p_name The name of the to be changed references.
         * @param p_arch_dia The to be assigned pointer to an architecture diagram.
         */
        void check_references( wxString p_name, architecture_diagram* p_arch_dia );

        /**
         * Process reference check function.
         * Given the specified name, finds all references that point to diagrams of the specified type and makes it point to the specified diagram pointer.
         * @param p_name The name of the to be changed references.
         * @param p_proc_dia The to be assigned pointer to a process diagram.
         */
        void check_references( wxString p_name, process_diagram* p_proc_dia );
      private:
        /**
         * Architecture diagram existance function.
         * @param p_name The base name of the architecture diagram.
         * @param p_index The index appearing in the architecture diagram's name.
         * @return Returns whether an architecture diagram with the specified name exists.
         */
        bool exists_architecture_diagram( const wxString &p_name, int p_index );

        /**
         * Process diagram existance function.
         * @param p_name The base name of the architecture diagram.
         * @param p_index The index appearing in the architecture diagram's name.
         * @return Returns whether a process diagram with the specified name exists.
         */
        bool exists_process_diagram( const wxString &p_name, int p_index );

        /**
         * Diagram existance function.
         * @param p_name The base name of the diagram.
         * @param p_index The index appearing in the diagram's name.
         * @return Returns whether a diagram with the specified name exists.
         */
        bool exists_diagram( const wxString &p_name, int p_index );
    };

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_GRAPESPECIFICATION_H
