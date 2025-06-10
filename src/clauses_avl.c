#include <clauses_avl.h>
/************************************/
/*** Implémentation des fonctions ***/
/************************************/

TNoeud *Pile[MAXP];	// Pile utilisée dans la recherche, l'insertion itérative et la suppression
int sommet = -1;		// initialisée à vide

/* si v existe, retourne dans p son adresse sinon NULL */
/* dans tous les cas la Pile contiendra tous les ascendants visités */
void rechercher( TVal v, TNoeud *r, TNoeud **p )
{
    int trouv;

    // Vider la pile ...
    sommet = -1;
    trouv = 0;
    *p = r;

    while ( !trouv && *p != NULL )
        if (clause_compare(Info(*p) , v) == 0 ) trouv = 1;
        else {
            // Empiler(*p) ...
            if ( sommet < MAXP ) Pile[ ++sommet ] = *p; else exit( EXIT_FAILURE );
            // ensuite descendre à gauche ou à droite ...
            if ( clause_compare(v , Info(*p)) < 0 )
                *p = fg(*p);
            else
                *p = fd(*p);
        }

} // rechercher


/***********************************/

bool inserer( TVal val, TNoeud **r )	// insertion itérative
{
   TNoeud *n, *q;
   int continu;

   // Phase de descente pour insérer la nouvele valeur ...
   rechercher( val, *r, &n );
   if ( n == NULL ) {
	n = CreerNoeud(); Aff_info(n,val); Aff_bal(n,0); Aff_fg(n,NULL); Aff_fd(n, NULL);
	if ( sommet == -1 ) // Si Pile vide ...
	   *r = n;
 	else {		    // Sinon ...
	   q = Pile[sommet--]; // Depiler(Pile,q)
	   if (clause_compare( val , Info(q))<0 )
		Aff_fg(q, n);
	   else
		Aff_fd(q, n);

	   // Phase de remontée pour la m-a-j des balances et de l'équilibrage ...
	   continu = 1;
	   while ( continu && q != NULL )
 	   {
		if ( n == fg(q) )  Dec_bal(q); else Inc_bal( q );

		switch( Bal(q) ) {
		   case -2 : if ( Bal( fg(q) ) == -1 ) {
		   		// rotation droite ...
				rotationD( &q );
		   	     }
		   	     else { // donc ( Bal( fg(q) ) == +1 )
		   		// double rotation gauche-droite ...
				// d'abord une rotation gauche sur n
				rotationG( &n );
				Aff_fg( q, n );
				// ensuite une rotation droite sur q
				rotationD( &q );
		   	     }
			     // M-a-j du père de q ...
			     if (sommet != -1) {
				n = Pile[sommet]; // n = le père de l'ancien q (avant rotation)
				if ( fg(n) == Pile[sommet+1] )
					Aff_fg(n, q);
				else
					Aff_fd(n, q);
			     }
			     else
				*r = q;

			     continu = 0;  // pour arrêter la remontée
			     break;

		   case +2 : if ( Bal( fd(q) ) == +1 ) {
		   		// rotation gauche ...
				rotationG( &q );
		   	     }
		   	     else { // donc ( Bal( fd(q) ) == -1 )
		   		// double rotation droite-gauche ...
				// d'abord une rotation droite sur n
				rotationD( &n );
				Aff_fd( q, n );
				// ensuite une rotation gauche sur q
				rotationG( &q );
		   	     }
			     // M-a-j du père de q ...
			     if (sommet != -1) {
				n = Pile[sommet]; // n = le père de l'ancien q (avant rotation)
				if ( fg(n) == Pile[sommet+1] )
					Aff_fg(n, q);
				else
					Aff_fd(n, q);
			     }
			     else
				*r = q;

			     continu = 0;  // pour arrêter la remontée
			     break;

		   case -1 : // dans les deux cas ( bal(q) == +1 ou -1 )
		   case +1 : // il faut continuer à remonter dans l'arbre
			     if ( sommet != -1 ) {  // si non pilevide ...
				n = q;
				q = Pile[sommet--];  // depiler(Pile, q)
			     }
			     else
				q = NULL;
			     break;

		   case 0  : continu = 0;  // pour arrêter la remontée
		} // switch

	   } // while

	} // else (si non Pilevide)
		return true ; // insertion réussie

   } // if (n == NULL)
   else return false ; // Valeur en double, pas d'insertion ...


} // inserer


/***********************************/

void rotationG( TNoeud **r )
{
	TNoeud *p = fd( *r );
	Aff_fd( *r, fg(p) );
	Aff_fg( p, *r );
	// m-a-j des balances ...
	Aff_bal( *r, Bal(*r)-1-max(Bal(p),0) );
	Aff_bal( p,  Bal(p)-1+min(Bal(*r),0) );

	*r = p;
}


/***********************************/

void rotationD( TNoeud **r )
{
	TNoeud *p = fg(*r);
	Aff_fg( *r, fd(p) );
	Aff_fd( p, *r );
	// m-a-j des balances ...
	Aff_bal( *r, Bal(*r)+1-min(Bal(p),0) );
	Aff_bal( p,  Bal(p)+1+max(Bal(*r),0) );

	*r = p;
}


/***********************************/

