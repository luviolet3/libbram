#ifndef LB_BRAM_HPP
#define LB_BRAM_HPP

#include <unordered_map>

#include "rapidxml-1.13/rapidxml.hpp"

namespace lb {
   class Parser {
    public:

    /**
     * Parses the contents of a .bram file
     * 
     * @param file the contents of the file
     */
    static void *parse(const char* file);

    private:

    /**
     * Parses a proof
     * 
     * @param proof the proof to parse
     *
     * @return the proof
     */
    static void parseProof(rapidxml::xml_node<> *proof, std::unordered_map<int, void*> &proofMap);

    /**
     * Parses an assumption
     * 
     * @param assumption the assumption to parse
     *
     * @return the proof
     */
    static void parseAssumption(rapidxml::xml_node<> *assumption);

    /**
     * Parses a step
     * 
     * @param step the step to parse
     *
     * @return the proof
     */
    static void parseStep(rapidxml::xml_node<> *step);

    /**
     * Parses a goal
     * 
     * @param goal the goal to parse
     *
     * @return the proof
     */
    static void parseGoal(rapidxml::xml_node<> *goal);

    /**
     * Parses a raw expression
     * 
     * @param raw the raw logic expression to parse
     *
     * @return the AST of the parsed expression
     */
    static void parseRaw(const char* raw);

    // Helper functions

    /**
     * Gets the value of a child node with certain name
     * 
     * @param node the node to search in
     * @param name the name of the node to search for
     * 
     * @return the value of the child node if found or nullptr if not found
     */
    static char* getNode(rapidxml::xml_node<> *node, const char* name);

    /**
     * Gets the value of an attribute with certain name
     * 
     * @param node the node to search in
     * @param name the name of the attribute to search for
     * 
     * @return the value of the attribute if found or nullptr if not found
     */
    static char* getAttribute(rapidxml::xml_node<> *node, const char* name);

    /**
     * Converts a string to an integer
     * 
     * @param __nptr the string to convert
     * 
     * @return the value as an integer or -1 if null string
     */
    static int atoiNullptr(const char* __nptr);

    /**
     * Gets the line number of a node
     * 
     * @param node the node to get the line number of
     * 
     * @return the line number or -1 if error
     */
    static int getLinenum(rapidxml::xml_node<> *node);
   };
}

#endif