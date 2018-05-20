#include <Basic_block.h>
#include <Enum_type.h>

//static
void Basic_block::show_dependances(Instruction *i1, Instruction *i2){

  if(i1->is_dep_RAW1(i2))
  cout<<"Dependance i"<<i1->get_index()<<"->i"<<i2->get_index()<<": RAW1"<<endl;
  if(i1->is_dep_RAW2(i2))
  cout<<"Dependance i"<<i1->get_index()<<"->i"<<i2->get_index()<<": RAW2"<<endl;

  if(i1->is_dep_WAR(i2))
  cout<<"Dependance i"<<i1->get_index()<<"->i"<<i2->get_index()<<": WAR"<<endl;

  if(i1->is_dep_WAW(i2))
  cout<<"Dependance i"<<i1->get_index()<<"->i"<<i2->get_index()<<": WAW"<<endl;

  if(i1->is_dep_MEM(i2))
  cout<<"Dependance i"<<i1->get_index()<<"->i"<<i2->get_index()<<": MEM"<<endl;

}

Basic_block::Basic_block():
Use(vector<bool>(NB_REG, false)),
LiveIn(vector<bool>(NB_REG, false)),
LiveOut(vector<bool>(NB_REG, false)),
Def(vector<bool>(NB_REG, false)),
DefLiveOut(vector<int>(NB_REG, -1)),
Domin(vector<bool>(NB_MAX_BB, false)){
  _head = NULL;
  _end = NULL;
  _branch = NULL;
  _index = 0;
  _nb_instr = 0;
  _firstInst=NULL;
  _lastInst=NULL;
  dep_done = false;
  use_def_done = false;
}


Basic_block::~Basic_block(){}


void Basic_block::set_index(int i){
  _index = i;
}

int Basic_block::get_index(){
  return _index;
}

void Basic_block::set_head(Line *head){
  _head = head;
}

void Basic_block::set_end(Line *end){
  _end = end;
}

Line* Basic_block::get_head(){
  return _head;
}

Line* Basic_block::get_end(){
  return _end;
}

void Basic_block::set_successor1(Basic_block *BB){
  _succ.push_front(BB);
}

Basic_block *Basic_block::get_successor1(){
  if (_succ.size()>0)
  return _succ.front();
  else
  return NULL;
}

void Basic_block::set_successor2(Basic_block *BB){
  _succ.push_back(BB);
}

Basic_block *Basic_block::get_successor2(){
  if (_succ.size()> 1)
  return _succ.back();
  else
  return NULL;
}

void Basic_block::set_predecessor(Basic_block *BB){
  _pred.push_back(BB);
}

Basic_block *Basic_block::get_predecessor(int index){

  list<Basic_block*>::iterator it;
  it=_pred.begin();
  int size=(int)_pred.size();
  if(index< size){
    for (int i=0; i<index; i++, it++);
    return *it;
  }
  else cout<<"Error: index is bigger than the size of the list"<<endl;
  return _pred.back();

}

int Basic_block::get_nb_succ(){
  return _succ.size();
}

int Basic_block::get_nb_pred(){
  return _pred.size();
}

void Basic_block::set_branch(Line* br){
  _branch=br;
}

Line* Basic_block::get_branch(){
  return _branch;
}

void Basic_block::display(){
  cout<<"Begin BB"<<endl;
  Line* element = _head;
  int i=0;
  if(element == _end)
  cout << _head->get_content() <<endl;

  while(element != _end->get_next()){
    if(element->isInst()){
      cout<<"i"<<i<<" ";
      i++;
    }
    if(!element->isDirective())
    cout <<element->get_content() <<endl;

    element = element->get_next();
  }
  cout<<"End BB"<<endl;
}

string Basic_block::get_content(){
  string rt = "";
  Line* element = _head;


  while(element != _end->get_next()){
    if(element->isInst()){
      rt = rt + element->get_content() + "\\l" ;
    }
    else if(element->isLabel())
    rt = rt + element->get_content() + "\\l" ;

    element = element->get_next();
  }

  return rt ;
}

int Basic_block::size(){
  Line* element = _head;
  int lenght=0;
  while(element != _end){
    lenght++;
    if(element->get_next()==_end)
    break;
    else
    element = element->get_next();
  }
  return lenght;
}


void Basic_block::restitution(string const filename){
  Line* element = _head;
  ofstream monflux(filename.c_str(), ios::app);
  if(monflux){
    monflux<<"Begin BB"<<endl;
    if(element == _end)
    monflux << _head->get_content() <<endl;
    while(element != _end)
    {
      if(element->isInst())
      monflux<<"\t";
      if(!element->isDirective())
      monflux << element->get_content()<<endl ;

      if(element->get_next()==_end){
        if(element->get_next()->isInst())
        monflux<<"\t";
        if(!element->isDirective())
        monflux << element->get_next()->get_content()<<endl;
        break;
      }
      else element = element->get_next();
    }
    monflux<<"End BB\n\n"<<endl;
  }
  else {
    cout<<"Error cannot open the file"<<endl;
  }
  monflux.close();

}

bool Basic_block::is_labeled(){
  if (_head->isLabel()){
    return true;
  }
  else return false;
}

int Basic_block::get_nb_inst(){
  if (_nb_instr == 0)
  link_instructions();
  return _nb_instr;

}

Instruction* Basic_block::get_instruction_at_index(int index){
  Instruction *inst;

  if(index >= get_nb_inst()){
    return NULL;
  }

  inst=get_first_instruction();

  for(int i=0; i<index; i++, inst=inst->get_next());

  return inst;
}

Line* Basic_block::get_first_line_instruction(){

  Line *current = _head;
  while(!current->isInst()){
    current=current->get_next();
    if(current==_end && !current->isInst())
    return NULL;
  }
  return current;
}

Instruction* Basic_block::get_first_instruction(){
  if(_firstInst==NULL){
    _firstInst= getInst(this->get_first_line_instruction());
    this->link_instructions();
  }
  return _firstInst;
}

Instruction* Basic_block::get_last_instruction(){
  if(_lastInst==NULL)
  this->link_instructions();
  return _lastInst;
}


/* link_instructions  numérote les instructions du bloc */
/* remplit le champ nb d'instructions du bloc (_nb_instr) */
/* remplit le champ derniere instruction du bloc (_lastInst) */
void Basic_block::link_instructions(){

  int index=0;
  Line *current, *next;
  current=get_first_line_instruction();
  next=current->get_next();

  Instruction *i1 = getInst(current);

  i1->set_index(index);
  index++;
  Instruction *i2;

  //Calcul des successeurs
  while(current != _end){

    while(!next->isInst()){
      next=next->get_next();
      if(next==_end){
        if(next->isInst())
        break;
        else{
          _lastInst = i1;
          _nb_instr = index;
          return;
        }
      }
    }

    i2 = getInst(next);
    i2->set_index(index);
    index++;
    i1->set_link_succ_pred(i2);

    i1=i2;
    current=next;
    next=next->get_next();
  }
  _lastInst = i1;
  _nb_instr = index;
}

bool Basic_block::is_delayed_slot(Instruction *i){
  if (get_branch()== NULL)
  return false;
  int j = (getInst(get_branch()))->get_index();
  return (j < i-> get_index());

}

/* set_link_succ_pred : ajoute succ comme successeur de this et ajoute this comme prédécesseur de succ
*/

void Basic_block::set_link_succ_pred(Basic_block* succ){
  _succ.push_back(succ);
  succ->set_predecessor(this);
}

/* add_dep_link ajoute la dépendance avec pred à la liste des dependances précédesseurs de succ */
/* ajoute la dependance avec succ à la liste des dépendances successeurs de pred */

/* dep est une structure de données contenant une instruction et  un type de dépendance */

void add_dep_link(Instruction *pred, Instruction* succ, t_Dep type){
  dep *d;
  d=(dep*)malloc(sizeof(dep));
  d->inst=succ;
  d->type=type;
  pred->add_succ_dep(d);

  d=(dep*)malloc(sizeof(dep));
  d->inst=pred;
  d->type=type;
  succ->add_pred_dep(d);
}


/* calcul des dépendances entre les instructions dans le bloc  */
/* une instruction a au plus 1 reg dest et 2 reg sources */
/* Attention le reg source peut être 2 fois le même */
/* Utiliser les méthodes is_dep_RAW1, is_dep_RAW2, is_dep_WAR, is_dep_WAW, is_dep_MEM pour déterminer les dépendances */

/* ne pas oublier les dépendances de controle avec le branchement s'il y en a un */

/* utiliser la fonction add_dep_link ci-dessus qui ajoute à la liste des dépendances pred et succ une dependance entre 2 instructions */


void Basic_block::comput_pred_succ_dep(){


  link_instructions();
  if (dep_done) return;
  Instruction *i_current;
  Instruction *itmp;

  /* A REMPLIR */
  for(int i = 0 ; i < get_nb_inst(); i++){
    i_current = get_instruction_at_index(i);

    for(int j=i+1; j < get_nb_inst(); j++){

      //dep RAW1
      if(i_current->is_dep_RAW1(get_instruction_at_index(j))){
        cout << i << " raw " << j << " RAW1" <<endl;
        add_dep_link(i_current,get_instruction_at_index(j),RAW);
      }

      //dep RAW2
      if(i_current->is_dep_RAW2(get_instruction_at_index(j))){
        cout << i << " raw " << j << " RAW2" <<endl;
        add_dep_link(i_current,get_instruction_at_index(j),RAW);
      }

      //dep WAR1
      if(i_current->is_dep_WAR1(get_instruction_at_index(j))){
        cout << i << " raw " << j << " WAR1" <<endl;
        add_dep_link(i_current,get_instruction_at_index(j),WAR);
      }

      //dep WAR2
      if(i_current->is_dep_WAR2(get_instruction_at_index(j))){
        cout << i << " raw " << j << " WAR2" <<endl;
        add_dep_link(i_current,get_instruction_at_index(j),WAR);
      }

      //dep MEM
      if(i_current->is_dep_MEM(get_instruction_at_index(j))){
        cout << i << " mem " << j <<endl;
        add_dep_link(i_current,get_instruction_at_index(j),MEMDEP);
      }


      //si dep WAW, on arrete
      if(i_current->is_dep_WAW(get_instruction_at_index(j))){
        cout << i << " waw " << j << " WAW : break" <<endl;
        break;
      }



    }
  }

  for(int i = 0 ; i < get_nb_inst(); i++){
    i_current = get_instruction_at_index(i);
    if(i_current->is_branch()){
      break;
    }
    //cout << i_current->to_string()<< " nb succ " << i_current->get_nb_succ() <<endl;
    if(i_current->get_nb_succ()==0 ){
      //cout << "isbranch " << get_instruction_at_index(get_nb_inst()-2)->to_string() << " " << get_instruction_at_index(get_nb_inst()-1)->is_branch()<<endl;
      if(get_instruction_at_index(get_nb_inst()-2)->is_branch()){
        add_dep_link(i_current,get_instruction_at_index(get_nb_inst()-2),CONTROL);
        cout << i << " control " <<endl;
      }
    }

  }



  // NE PAS ENLEVER : cette fonction ne doit être appelée qu'une seule fois
  dep_done = true;
  return;
}

void Basic_block::reset_pred_succ_dep(){

  Instruction *ic=get_first_instruction();
  while (ic){
    ic -> reset_pred_succ_dep();
    ic = ic -> get_next();
  }
  dep_done = false;
  return;
}

/* calcul le nb de cycles pour executer le BB, on suppose qu'une instruction peut sortir du pipeline à chaque cycle, il faut trouver les cycles de gel induit par les dépendances */

int Basic_block::nb_cycles(){

  Instruction *ic=get_first_instruction();

  /* tableau ci-dessous utile pour savoir pour chaque instruction quand elle sort pour en déduire les cycles qu'elle peut induire avec les instructions qui en dépendent, initialisation à -1  */

  vector<int> inst_cycle(get_nb_inst());
  for (int i=0; i< get_nb_inst(); i++ ){
    inst_cycle[i] = -1;
  }
  comput_pred_succ_dep();



   /* A REMPLIR */
  inst_cycle[0] = 1;

  for(int i=1; i<get_nb_inst(); i++){
    Instruction* i_instr = get_instruction_at_index(i);

    //calcul de max( { cycle(P)+delai(Pij) } ) ou P les dependances de i_instr
    int max_delay = 0;
    
    for(int j=0; j<i_instr->get_nb_pred(); j++){
      dep* current = i_instr->get_pred_dep(j);
      int delay = 0;

      if(current->type == RAW){
        //cout<<" | TYPEDEP: RAW";
        //cout<<" | DELAY: "<<delai(current->inst->get_type(), i_instr->get_type())<<endl;

        delay = delai(current->inst->get_type(), i_instr->get_type());

      }
      else if(current->type == MEMDEP){
        //cout<<" | TYPEDEP: MEM";
        //cout<<" | DELAY: 1"<<endl;

        delay = 1;

      }
      else if(current->type == NONE){
        cout<<"DEPENDANCE NONE ?????????????????????????????????????????"<<endl;
      }
      else{
        //cout<<" | TYPEDEP: WAR, WAW or CONTROL";
        //cout<<" | DELAY: 0"<<endl;

        delay = 0;

      }
      //cout<<" delay_i= "<< inst_cycle[current->inst->get_index()] + delay<<endl;
      max_delay = max(max_delay, inst_cycle[current->inst->get_index()] + delay);

    }

    //calcul de cycle(i) = max( cycle(i-1)+1, max_delay)
    inst_cycle[i] = max(inst_cycle[i-1]+1, max_delay);

  }

  cout<<"instr : num cycle de sortie"<<endl;
  for(int i=1; i<get_nb_inst(); i++){
    cout<<"i"<<i<<": "<<inst_cycle[i]<<endl;
  }


 return inst_cycle[get_nb_inst()-1];
}

/*
calcule DEF et USE pour l'analyse de registre vivant
à la fin on doit avoir
USE[i] vaut 1 si $i est utilisé dans le bloc avant d'être potentiellement défini dans le bloc
DEF[i] vaut 1 si $i est défini dans le bloc
ne pas oublier les conventions d'appel : les registres $4, $5, $6, $7 peuvent contenir des paramètres (du 1er au 4eme les autres sont sur la pile) avant un appel de fonctions, au retour d'une fonction $2 a été écrit car il contient la valeur de retour (sauf si on rend void). Un appel de fonction (call) écrit aussi l'adresse de retour dans $31.

******************/

void Basic_block::compute_use_def(void){
  Instruction * inst = get_first_instruction();
  if (use_def_done) return;

  cout << "====================== Compute def use ==================== " << endl;

  //Use
  for(int i = 0; i < get_nb_inst();i++){

    inst = get_instruction_at_index(i);
    //cout << inst->to_string();
    if(inst->get_reg_dst()!=NULL){
    // cout << "def : " << inst->get_reg_dst()->get_reg()<<endl;
      if(Def[inst->get_reg_dst()->get_reg()]==true){

      }
      else{
        Def[inst->get_reg_dst()->get_reg()]=true;
      //  cout << " DEF : " << inst->get_reg_dst()->get_reg();
      }
    }

    if(inst->is_call()){
      // cout << "op " << inst->get_op_label()->to_string()<< "--------------------------------------" << endl;
      Def[31]=true;
      Def[2]=true;
      // cout << " def 4 " << Def[4] << endl;
      // cout << " def 5 " << Def[5] << endl;
      // cout << " def 6 " << Def[6] << endl;
      if(Def[4]==false){ Use[4] = true; }
      if(Def[5]==false){ Use[5] = true; }
      if(Def[6]==false){ Use[6] = true; }
  }

    if(inst->get_reg_src1()!=NULL){
    //   cout << "use : " << inst->get_reg_src1()->get_reg()<<endl;
    if(Use[inst->get_reg_src1()->get_reg()]==true){
      }
      if(Def[inst->get_reg_src1()->get_reg()] != NULL && Def[inst->get_reg_src1()->get_reg()]==true){
      }

      else{
        Use[inst->get_reg_src1()->get_reg()]=true;
      //  cout << " USE " << inst->get_reg_src1()->get_reg();
      }
    }

    if(inst->get_reg_src2()!=NULL){
    //  cout << "use : " << inst->get_reg_src2()->get_reg()<<endl;
      if(Use[inst->get_reg_src2()->get_reg()]==true){
      }

      if(Def[inst->get_reg_src2()->get_reg()]!= NULL && Def[inst->get_reg_src2()->get_reg()]==true){
      }

      else{
        Use[inst->get_reg_src2()->get_reg()]=true;
        //  cout << " USE " << inst->get_reg_src1()->get_reg();
      }
    }




}

  Use[0]=true;





  cout << "****** BB " << get_index() << "************" << endl;
  cout << "USE : " ;
  for(int i=0; i<NB_REG; i++){
    if (Use[i])
    cout << "$"<< i << " ";
  }
  cout << endl;
  cout << "DEF : " ;
  for(int i=0; i<NB_REG; i++){
    if (Def[i])
    cout << "$"<< i << " ";
  }
  cout << endl;

  return;

}

/**** compute_def_liveout
à la fin de la fonction on doit avoir
DefLiveOut[i] vaut l'index de l'instruction du bloc qui définit $i si $i vivant en sortie seulement
Si $i est défini plusieurs fois c'est l'instruction avec l'index le plus grand
*****/
void Basic_block::compute_def_liveout(){

  /* A REMPLIR */

  //pour tous les registres
  for(int i=0; i<NB_REG; i++){

    //si le registre i est defini dans une des instructions du bloc
    // et s'il est vivant en sortie de bloc
    if(this->Def[i] && this->LiveOut[i]){

      Instruction * inst = get_first_instruction();

      //pour toutes les instructions du bloc
      while(inst != NULL){

        //si l'instruction definit le registre i
        if(inst->get_reg_dst()!=NULL && inst->get_reg_dst()->get_reg()==i){
          //alors on ajoute l'index de l'instruction a la case DefLiveOut[i].
          //l'index de la derniere instruction a definir le registre sera conserve.
          this->DefLiveOut[i] = inst->get_index();
        }

        inst = inst->get_next();
      }

    }   
  }


  #ifdef DEBUG
  cout << "DEF LIVE OUT: "<<endl ;
  for(int i=0; i<NB_REG; i++){
    if (DefLiveOut[i] != -1)
    cout << "$"<< i << " defini par i" << DefLiveOut[i] << endl;
  }

  #endif
}



/**** renomme les registres renommables : ceux qui sont définis et utilisés dans le bloc et dont la définition n'est pas vivante en sortie
Utilise comme registres disponibles ceux dont le numéro est dans la liste paramètre
*****/
void Basic_block::reg_rename(list<int> *frees){
  Instruction * inst = get_first_instruction();
  int newr;
  compute_def_liveout();


  /* A REMPLIR */

  list<int> renommables;

  //boucle pour trouver les registres renommables.
  for(int i =0; i< NB_REG; i++){

    //si le registre est defini dans le bloc mais est mort en sortie du bloc
    if(this->Def[i] && !(this->LiveOut[i])){
      //ajout du registre a la liste des registres renommables
      renommables.push_back(i);
    }
  }

  //boucle de renommage.
  while( !(renommables.empty()) ){
    int reg_a_renommer = renommables.front();
    renommables.pop_front();

    int reg_nouv_nom = frees->front();
    frees->pop_front();

    //on trouve la 1e definition du registre a renommer
    inst = get_first_instruction();
    while( !(inst->get_reg_dst()!=NULL && 
             inst->get_reg_dst()->get_reg()==reg_a_renommer) ){
      inst = inst->get_next();
    }

    //une fois trouve, on renomme le registre dans sa defition.
    inst->get_reg_dst()->set_reg(reg_nouv_nom);

    //puis on renomme les utilisations ulterieures 
    //jusqu'a la fin du BB ou la prochaine definition du registre (voir 3e if)
    inst = inst->get_next();
    while(inst != NULL){
      if(inst->get_reg_src1()!=NULL && inst->get_reg_src1()->get_reg()==reg_a_renommer){
        inst->get_reg_src1()->set_reg(reg_nouv_nom);
      }
      if(inst->get_reg_src2()!=NULL && inst->get_reg_src2()->get_reg()==reg_a_renommer){
        inst->get_reg_src2()->set_reg(reg_nouv_nom);
      }

      //(on verifie le registre que definit l'instr apres ceux qu'elle utilise)
      if(inst->get_reg_dst()!=NULL && inst->get_reg_dst()->get_reg()==reg_a_renommer){
        break;
      }

      inst = inst->get_next();
    }
  }

}


/**** renomme les registres renommables : ceux qui sont définis et utilisés dans le bloc et dont la définition n'est pas vivante en sortie
Utilise comme registres disponibles ceux dont le numéro est dans la liste paramètre
*****/
void Basic_block::reg_rename(){
  Instruction * inst = get_first_instruction();
  int newr;
  list<int> *frees, lfree;


  /* A REMPLIR */

  //boucle pour trouver les registres disponibles.
  for(int i =0; i< NB_REG; i++){

    //si le registre est mort en entree du bloc et n'est pas defini dans le bloc 
    if( !(this->LiveIn[i]) && !(this->Def[i]) ){
      //ajout du registre a la liste des registres disponibles
      frees->push_back(i);
    }
  }

  //on supprime de la liste les registres reserves en MIPS (0, 1, 26, 27)
  frees->remove(0); //special, toujours 0. on lenleve par securite
  frees->remove(1); //reserve pour l'assembleur
  frees->remove(26); //reserve par l'OS
  frees->remove(27); //reserve par l'OS

  //on lance la fonction precedente en donnant notre liste de reg dispo en arg.
  reg_rename(frees);
}

void Basic_block::apply_scheduling(list <Node_dfg*> *new_order){
  list <Node_dfg*>::iterator it=new_order->begin();
  Instruction *inst=(*it)->get_instruction();
  Line *n=_head, *prevn=NULL;
  Line *end_next = _end->get_next();
  if(!n){
    cout<<"wrong bb : cannot apply"<<endl;
    return;
  }

  while(!n->isInst()){
    prevn=n;
    n=n->get_next();
    if(n==_end){
      cout<<"wrong bb : cannot apply"<<endl;
      return;
    }
  }

  //y'a des instructions, on sait pas si c'est le bon BB, mais on va supposer que oui
  inst->set_index(0);
  inst->set_prev(NULL);
  _firstInst = inst;
  n = inst;

  if(prevn){
    prevn->set_next(n);
    n->set_prev(prevn);
  }
  else{
    set_head(n);
  }

  int i;
  it++;
  for(i=1; it!=new_order->end(); it++, i++){

    inst->set_link_succ_pred((*it)->get_instruction());

    inst=(*it)->get_instruction();
    inst->set_index(i);
    prevn = n;
    n = inst;
    prevn->set_next(n);
    n->set_prev(prevn);
  }
  inst->set_next(NULL);
  _lastInst = inst;
  set_end(n);
  n->set_next(end_next);
  return;
}

/* permet de tester des choses sur un bloc de base, par exemple la construction d'un DFG, à venir ... là ne fait rien qu'afficher le BB */
void Basic_block::test(){
  cout << "test du BB " << get_index() << endl;
  display();
  cout << "nb de successeur : " << get_nb_succ() << endl;
  int nbsucc = get_nb_succ() ;
  if (nbsucc >= 1 && get_successor1())
  cout << "succ1 : " << get_successor1()-> get_index();
  if (nbsucc >= 2 && get_successor2())
  cout << " succ2 : " << get_successor2()-> get_index();
  cout << endl << "nb de predecesseurs : " << get_nb_pred() << endl ;

  int size=(int)_pred.size();
  for (int i = 0; i < size; i++){
    if (get_predecessor(i) != NULL)
    cout << "pred "<< i <<  " : " << get_predecessor(i)-> get_index() << "; ";
  }
  cout << endl;
}
