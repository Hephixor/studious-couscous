#include <iostream>
#include <Program.h>
#include <Basic_block.h>
#include <Function.h>


int main(int argc, char * argv[]){

	if (argc < 2) {
	  cout << "erreur : pas de fichier assembleur" << endl;
	}	  
	
	// moche mais pour nommer mieux les fichiers dot en sortie avec nom du fichier uniquement (pas le chemin) et sans extension
	int ii, debut = 0;
	for (ii = 0; ii < strlen(argv[1]); ii++){
	  if (argv[1][ii] == '\/') debut = ii+1;
	  if (argv[1][ii] == '\.') break;
	}
	char * name = (char *) malloc(sizeof(char) * ii +1);
	strncpy(name, argv[1]+debut, ii-debut);
	name[ii] = '\0';

	
	Program prog(argv[1]);
	Function* functmp;
	list <Function*> myfunc; 
	list <Basic_block*> myBB;

	cout<<"Le programme a "<<prog.size()<<" lignes\n"<<endl;

	// delimitation et construction des fonctions du fichier
	prog.comput_function();
	cout<<"nombre de fonctions : "<<prog.nbr_func()<<endl;

	list<Function*>::iterator itfct;
	list<Basic_block*>::iterator itbb;
	Basic_block *bb;
	int i, j;
	list<int> frees;
	Dfg *d;
	Cfg *c;
	
	std::ostringstream *oss ;

	//traitement pour toutes les fonction 
	for(itfct=prog.function_list_begin(), i=0;
	    itfct!=prog.function_list_end(); itfct++, i++){

	   functmp=*itfct;
	   cout<<"------------Function DISPLAY----------\n" <<endl;
	   functmp->display();
	   
	   // delimitation des BB de la fonction
	   functmp->comput_basic_block();
	   // association des labels avec le bloc qui le suit
	   functmp->comput_label();
	   // determination des liens entre les BB
	   functmp->comput_succ_pred_BB();
	   
	   // production d'un fichier .dot pour le CFG de la fonction
	   oss=new std::ostringstream;	   
	   (*oss)<<"./tmp/"<< name << "_cfg_func_" << i<<".dot";
	   c=new Cfg(functmp->get_BB(0), functmp->nbr_BB());
	   c->restitution(NULL, oss->str());
	   
	   cout<<"========== Function "<<i<<"==========="<<endl;
	   cout<<"============================"<<endl;
	   
	   // calcul des registres vivants en entrée et sortie des BB
	   functmp ->compute_live_var();
	   j=0;
	   
	   // iteration sur tous les BB du CFG
	   for(itbb=functmp->bb_list_begin(); 
	       itbb!=functmp->bb_list_end(); itbb++, j++){
	     
	     bb=*itbb;
	     cout<<"----------BB "<<j<<"-----------"<<endl;
	     
	     //affichage du BB
	     bb->display();
	     // création des liens entre les instructions du BB 
	     bb->link_instructions();

	     //calcul des liens de dépendances entre les instructions du BB
	      bb->comput_pred_succ_dep();

	      /**************** CALCUL NB CYCLES *********************/
	      //affichage du nb de cycle pour l'execution du BB
	      cout<<"---nb_cycles : "<<bb->nb_cycles()<<"-----------"<<endl;

	      // creation du DAG de dépendance associé au BB
	      d = new Dfg(bb);

	     
	      string str;
	      stringstream sstm;
	      sstm << "./tmp/" << name << "_func_" << i << "_dfg_bb" << bb->get_index() <<".dot";
	      str = sstm.str();
	      d->restitute(NULL, str, true);

	      /*********** CALCUL CHEMIN CRITIQUE  ***********/ 

	      cout<<"comput critical path"<<endl;
	      d->comput_critical_path();
	      cout<<"critical path "<<d->get_critical_path()<<endl;
	      
	      /************** REORDONNANCEMENT *****************/ 

	      // calcul d'un nouvel ordonnancement

	      d->scheduling(false);

	      // DECOMMENTER pour afficher le nouvel ordonnancement

	      //cout<<"----  new scheduling: -----"<<endl;
	      //d->display_sheduled_instr();

	      // Application du  nouvel ordonnancement (changement réel dans le BB)
	      //d->apply_scheduling();
	      // cout<<"---- BB with new scheduling: -----"<<endl;
	      // bb->display();

	      // recalcul du nb de cycle après scheduling 
	      // cout<<"---nb_cycles : "<<bb->nb_cycles()<<"-----------"<<endl;
	   
	      
	      /**************** RENOMMAGE DE REGISTRE ****************/ 

	      // liste de registres pour le renommage
	      // avec des registres passéees en paramètre 
	      /*
	      frees.clear();
	      for(int k=32; k<64; k++){
	      	 frees.push_back(k);
	      }
	      */

	      /* renommage en utilisant des registres n'existant pas */
	      
	      //  bb->reg_rename(&frees);

	      /* renommage utilisant les registres disponibles dans le bloc */
	      /*  ne pas faire les 2 */
	      /* il faut recalculer les informations de vivacité et de def-use 
	         pour pouvoir le faire 2 fois de suite !!
	      */

	      bb->reg_rename();
	      cout<<"----- apres renommage ------"<<endl;
	      bb->display();

	      // il faut annuler le calcul des dépendances et le refaire
	      bb->reset_pred_succ_dep();
	      bb->comput_pred_succ_dep();
	      cout<<"---nb_cycles--"<<bb->nb_cycles()<<"-----------"<<endl;

	      d= new Dfg(bb);
	      cout<<"comput critical path"<<endl;
	      //d->restitute(NULL, "./tmp/graph_dfg1.dot", true);
	      d->comput_critical_path();
	      cout<<"critical path "<<d->get_critical_path()<<endl;

	      // creation du DAG en version .dot avec le renommage 
	      stringstream sstm2;
	      sstm2 <<    sstm << "./tmp/" << name << "_func_" << i << "_dfg_bb" << bb->get_index() <<"_renamed.dot";
	      string str2 = sstm2.str();
	      cout << str2 << endl;
	      d->restitute(NULL, str2, true);

	      // scheduling sur le code renommé
	      d->scheduling(false);
	      //cout<<"----  new scheduling: -----"<<endl;
	      //d->display_sheduled_instr();
	      d->apply_scheduling();
	      cout<<"---- BB with new scheduling: -----"<<endl;
	      bb->display();
	     
	      // nombre de cycles du BB après renommage et scheduling 
	      cout<<"----nb_cycles--"<<bb->nb_cycles()<<"-----------"<<endl;
	      //return 0;
	   }
	}
	    
	
	

	
}
