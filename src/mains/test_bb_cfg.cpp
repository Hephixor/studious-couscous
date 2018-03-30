

#include <iostream>
#include <Program.h>
#include <Basic_block.h>
#include <Function.h>


/*
  ##################Programme � appeler avec le nom d'un fichier assembleur en argument###############

  -Affichage du nombre de lignes du programme, de son contenu
  - D�coupage en fonctions et affichage du nombre de fonction
  - D�coupage en nombre de bloc de base
  -Pour chaque fonction, calcul du decoupage en blocs de base, des successeurs et pr�d�cesseurs des BB, construction et restitution en fichier .dot du CFG
  -Affichage du BB0 de la fonction0

*/

int main(int argc, char * argv[]){

   remove("tmp/test1.txt");
   remove("tmp/test2.txt");
   remove("./tmp/graph.dot");
   if (argc < 2) {
      cout << "erreur : pas de fichier assembleur en entree" << endl;
   }

   // moche mais utile pour r�cuperer le nom du fichier dans le chemin entr�e et sans extension
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

   list <Basic_block*> myBB;

   cout<<"Le programme a "<<prog.size()<<" lignes\n"<<endl;

   cout<<"Contenu du programme:"<<endl;
   prog.display();
   
   
   cout<<"\n Calcul des fonctions des block de base et restitution\n"<<endl;
   prog.comput_function();
   

   cout<<"nombre de fonctions : "<<prog.nbr_func()<<endl;

   Cfg *graph;
   for (int i=0; i<prog.nbr_func(); i++){
      
     // recup�ration de la ieme fonction du programme
      functmp= prog.get_function(i);

      if(functmp==NULL){
	 cout<<"null"<<endl;
	 break;
      }
      cout << "FONCTION " << i << endl;
      // decommenter pour voir le code de la fonction dans le terminal
      //functmp -> display();

      //determine les BB de la fonction
      functmp->comput_basic_block();
      
      // commenter ci-dessous pour ne pas voir le d�coupage en BB
      cout << "Affichage des blocs de base " << endl; 
      for(int j=0; j<functmp->nbr_BB(); j++){
	 functmp->get_BB(j)->display();
      }
     
      // calcul les �tiquettes de la fonction 
      functmp->comput_label();
      // calcul les liens entre BB 
      functmp->comput_succ_pred_BB();

      // appelle la fonction test (voir Function.cpp)
      // affichage des BB et des predecesseurs/successeurs
      functmp->test();

      //calcule des dominants
      functmp->compute_dom();

      // cr�e un nouveau CFG pour la fonction
      // permet de le visualiser avec dotty, cf. plus bas
      graph =new Cfg(functmp->get_BB(0),
			  functmp->nbr_BB());
	
		
      cout<<"------------Function "<< (i+1) <<"/"<<prog.nbr_func()<<" DISPLAY----------\n" <<endl;
    
      // pour afficher le CFG mais l'affichage ci-dessus donne les infos aussi
      // graph->display(NULL);

      // construit un fichier .dot pour le cfg numero i dans ./tmp/
      string str="./tmp/";
      stringstream sstm;
      sstm << "tmp/cfg_graph_" << name << "_" << i <<".dot";
      str = sstm.str();
      cout << str << endl;
      graph->restitution(NULL,str);
   }
  
   return 0;
}
