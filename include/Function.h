#ifndef _Function_H
#define _Function_H

/**	\file	Function.h
	\brief	Function class
	\author	Hajjem
*/


#include <Line.h>
/*#include <Directive.h>
  #include <Instruction.h>*/
#include <Basic_block.h>
#include <Instruction.h>
#include <string>
#include <stdio.h>
#include <Label.h>
#include <Enum_type.h>
#include <list>
#include <Cfg.h>
#include <fstream>


using namespace std;

/**	\class	Function
	\brief	class representing a Function on a program
*/

class Function{

public:
/**	\brief	Constructor of a function
*/
	Function();

/**	\brief	Destructor of a function
*/
	~Function();

/**	\brief	setter of the head of the function
*/
	void set_head(Line *);

/**	\brief	setter of the end of the function
*/
	void set_end(Line *);

/**	\brief	get the head of the function
*/
	Line* get_head();

	Basic_block *get_firstBB();

/**	\brief	get the ending Line of the function
*/
	Line* get_end();

/**	\brief	display the function
*/
	void display();

/**	\brief	get number of Lines  of the function
*/
	int size();	

/**	\brief	restitute the function in a file
*/
	void restitution(string const);

/**	\brief creates a new BB with the given start line, end line and branch line and its index, add it to the BB list of this
*/
	void add_BB(Line *, Line*, Line*, int);
/**	\brief	Calculate the basics bolck of the function
*/
	void comput_basic_block();

/**	\brief	get the number of Basic block in the function
*/
	int nbr_BB();

/**	\brief	get the Basic Block at a position in the BB list
*/
	Basic_block *get_BB(int);
	
	/**  \brief iterators of the BB list 
	 */
	list<Basic_block*>::iterator bb_list_begin();
	list<Basic_block*>::iterator bb_list_end();

/**	\brief	comput labels of the function in list
*/

	void comput_label();

/**	\brief	get a specific label of the function
*/
	Label* get_label(int);

/**	\brief	get the size of the list label
*/
	int nbr_label();

/**	\brief	Get the basic block that starts with a given label (operand)
*/
	Basic_block *find_label_BB(OPLabel*);

/**	\brief	Computes the successors and predecessors of each Basic block 
*/
	void comput_succ_pred_BB();

/**	\brief	method to perform some test, usefull for testing methods on basic blocks
*/
	void test();
	/** \brief computes live variable for each basic blocks
	 */
	void compute_live_var();

	/** \brief computes dominators for each basic blocks 
	 */

	void compute_dom();
private:
	/** \brief true if basic blocks has been computed
	 */
	bool BB_computed;

	/** \brief true if all basic block links have been computed
	 */

	bool BB_pred_succ;

	/** \brief true if dominators has been computed
	 */

	bool dom_computed;

	Line *_head;
	Line *_end;

	/** \brief list of basic blocks of the function
	 */


	list <Basic_block*> _myBB;


	/** \brief list of labels of the function
	 */

	list <Label*> _list_lab;
};

#endif
