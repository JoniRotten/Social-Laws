//apliquem social laws
//apliquem que no es pot anar a una casella plena si estas a l'inici, i una llei massa restrictiva al mig, que no es pot moure si hi ha cotxe a qualsevol de les caselles que es vol anar

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>



#define N 135656    	 //1000 més del que em donava de resultat
#define S 4096	       	 //vectors nocrash
#define T 4096     	 //vectors nocrash despres d'eliminar simetrics
#define C 135656    	 //transicions crash, ho deixem com les totals


int * vector(int);
struct position * svector(int);
struct transition * stvector(int);
struct collision * scvector(int);
struct position ** svectorp(int);
struct collision ** scvectorp(int);
int ** matrix(int, int);
void freem(int **,int);
void printv(int *, int);
void fprintv(int *, int, FILE *);
int compare(int *, int *);  
int treenextsl(int *,int *);
void treesl(int *,int *, int **, int);
int grid(struct position *,struct position **, struct position **);
int * rotate(int *, int);
void symmetry(struct position **, struct position **);
int numtran(struct position **);
void movetran(int *, int *, int, int);
int movecount(struct transition);
int * crashcount(struct transition);
void filltran(struct position **, struct transition *);
int numcol(struct transition *, int);
void fillcol(struct transition *, struct collision *, int);
void printfilesl(int, struct position *, FILE *,FILE *);
void clascol(int, struct collision *, FILE *);


int nextp[12][2]={{8,-111},{-111,-111},{9,-111},{-111,-111},{10,-111},{-111,-111},{11,-111},{-111,-111},{9,1},{10,3},{11,5},{8,7}};  //amb un define??
//matriu de moviments

//int mlook[16][2]={{11,8},{-111,-111},{8,9},{-111,-111},{9,10},{-111,-111},{10,11},{-111,-111},{9,2},{10,4},{11,6},{8,0},{1,-111},{3,-111},{5,-111},{7,-111}};
//matriu de les possicions a mirar per moure's, les possicions centrals tenen dos vectors si es mouen al centre o de la 12 a 15 si surten
//l'order és mirant a la casella que vols anar esquerra-centre-dreta

int mlook[12]={11,-111,8,-111,9,-111,10,-111,2,4,6,0};
//es un vector que indica quina quan un cotxe a una casella es mou, quins altres cotxes poden moure's a la mateixa.
//no tinc en compte que hi hagi un cotxe a la casella
//per aixó les possicions centrals son només dels cotxes que es mouen al centre


struct position {
			int * pos;				//vector de possició
			int * branch;				//vector amb el nombre de branques next per cada possició
			int npos;				//identificador de possició
                	struct position ** next;		//vector de punters a totes les possicions
                	int nnext;				//número de transicions next
                	int nprev;				//número de transicions prèvies
                	int nextcrash;      		 	//nombre de transicions crash, no estic recollint aquesta informació
                	int iscrash;                  		//0 si no es crash 1 si es crash
		};


struct transition {
			struct position * prev;			//node de surtida
			struct position * next;			//node arrivada
			int * move;				//vector de 16 components que indica a on va cada casella
			int iscrash;				//0 es transició nocrash 1 es transició crash
			int nmove;				//número de moviments
//			int * vmove;				//0 si no s'ha mogut la casella 1 si s'ha mogut POSIBLE CAUSANTE DE FALTA DE MEMORIA
			int * ncrash;				//vector amb número de crash, crash al inici, crash al mig, crash al final
		  };


struct collision {
			struct transition * tran;		//punter a la transsició
			int pos;				//casella de la col-lisió
			int isempty;				//0 si la casella pos es vuida perviament, 1 sinó
			int ismove;				//si la casella no es vuida i es mou 0, si no es mou 1 si es vuida -111
			int ncar;				//cotxes implicatas a la col-lisió
			int * pprev;				//vector de 2 components que indica els cotxes que s'han mogut a la casella
								//l'ordre és inici centre
			int * look;				//casselles a mirar als que s'han mogut ordre inici centre
		 };



void main(void)
{
        int nnode, ntran, ncol;					//número de nodes, transicions, colisions			
        struct position *node;					//vector de nodes
	struct transition *vtran;				//vector de transicions
	struct collision *vcol;					//vector de colisions
        struct position **crash, **nocrash, **nocrashs;		//vectors de punters a nodes crash, nodes nocrash i nocrash no simètrics
        FILE *pf,*qf,*rf,*sf,*tf,*cf;
        time_t timer;
        pf=fopen("sl_positions.dat","w+");
        qf=fopen("sl_crash.dat","w+");
        /*rf=fopen("new_transitions_1.dat","w+");
        tf=fopen("new_transitions_2.dat","w+");
        sf=fopen("new_info.dat","w+");
	cf=fopen("colisions.dat","w+");*/
        if(pf==NULL)
        {
                printf("\nno puc obrir el fitxer!!!!!");
                exit(1);
        }
        if(qf==NULL)
        {
                printf("\nno puc obrir el fitxer!!!!!");
                exit(1);
        }
        /*if(rf==NULL)
        {
                printf("\nno puc obrir el fitxer!!!!!");
                exit(1);
        }
        if(sf==NULL)
        {
                printf("\nno puc obrir el fitxer!!!!!");
                exit(1);
        }
        if(tf==NULL)
        {
                printf("\nno puc obrir el fitxer!!!!!");
                exit(1);
        }
        if(cf==NULL)
        {
                printf("\nno puc obrir el fitxer!!!!!");
                exit(1);
        }*/
        node=svector(N);
	crash=svectorp(C);
        nocrash=svectorp(S);
        //nocrashs=svectorp(T);
        timer=time(NULL);
        printf("\n\n\nThe current time is %s.\n\n\n",asctime(localtime(&timer)));
        nnode=grid(node, crash, nocrash);
        /*timer=time(NULL);
        printf("\n\n\nThe current time is %s.\n\n\n",asctime(localtime(&timer)));*/
//        symmetry(nocrash,nocrashs);
//        timer=time(NULL);
//        printf("\n\n\nThe current time is %s.\n\n\n",asctime(localtime(&timer)));
	ntran=numtran(nocrash);
	printf("\n\n\n%d\n\n\n",ntran);
	/*vtran=stvector(ntran);
	filltran(nocrash,vtran);
        timer=time(NULL);
        printf("\n\n\nThe current time is %s.\n\n\n",asctime(localtime(&timer)));
	ncol=numcol(vtran,ntran);
	vcol=scvector(ncol);
	fillcol(vtran,vcol,ntran);
        timer=time(NULL);
        printf("\n\n\nThe current time is %s.\n\n\n",asctime(localtime(&timer)));*/
        printfilesl(nnode,node,pf,qf);
        timer=time(NULL);
        printf("\n\n\nThe current time is %s.\n\n\n",asctime(localtime(&timer)));
/*	clascol(ncol,vcol,cf);
        timer=time(NULL);
        printf("\n\n\nThe current time is %s.\n\n\n",asctime(localtime(&timer)));*/
        fclose(pf);
        fclose(qf);
/*        fclose(rf);
        fclose(sf);*/
        free(node);
/*	free(vtran);
	free(vcol);*/
}


int * vector(int n)
{
        int *v;
        v=(int *) calloc(n,sizeof(int));
        if(v==NULL)
        {
                printf("\nNo hi ha memoria pel vector!!!!!!!!!!!");
                exit(1);
        }
        return v;
}


struct position * svector(int n)
{
        struct position *v;
        v=(struct  position *) calloc(n,sizeof(struct position));
        if(v==NULL)
        {
                printf("\nNo hi ha memoria per l'estructura de posicions!!!!!!!!!!");
                exit(1);
        }
        return v;
}


struct transition * stvector(int n)
{
        struct transition *v;
        v=(struct  transition *) calloc(n,sizeof(struct transition));
        if(v==NULL)
        {
                printf("\nNo hi ha memoria per l'estructura de transicions!!!!!!!!!!");
                exit(1);
        }
        return v;
}


struct collision * scvector(int n)
{
        struct collision *v;
        v=(struct  collision *) calloc(n,sizeof(struct collision));
        if(v==NULL)
        {
                printf("\nNo hi ha memoria per l'estructura de col-lisions!!!!!!!!!!");
                exit(1);
        }
        return v;
}


struct position ** svectorp(int n)
{
        struct position **v;
        v=(struct position **) calloc(n,sizeof(struct position *));
        if(v==NULL)
        {
                printf("\nNo hi ha memoria pel vector d'estructures!!!!!!!!!!");
                exit(1);
        }
        return v;
}


struct collision ** scvectorp(int n)
{
        struct collision **v;
        v=(struct collision **) calloc(n,sizeof(struct collision *));
        if(v==NULL)
        {
                printf("\nNo hi ha memoria pel vector d'estructures!!!!!!!!!!");
                exit(1);
        }
        return v;
}


int ** matrix(int n,int m)
{
        int i;
        int **a;
        a=(int **) calloc(n,sizeof(int*));
        if(a==NULL)
        {
                printf("\nNo hi ha memoria per la matriu!!!!!!!!!!!");
                exit(1);
        }
        for(i=0;i<n;i++)
                a[i]=vector(m);
        return a;
}



void freem(int **a,int n)
{
        int i;
        for(i=0;i<n;i++)
                free(a[i]);
        free(a);
}



void printv(int *v, int n)
{
        int i;
        printf("\n");
        for(i=0;i<n;i++)
                printf("%d\t",v[i]);
}


void fprintv(int *v, int n, FILE * pf)
{
        int i;
        fprintf(pf,"\n");
        for(i=0;i<n;i++)
                fprintf(pf,"%d\t",v[i]);
}


int compare(int *v, int *w)
	//retorna 0 si son iguals 1 si son diferents (podria retornar les posicions crash)
{
        int i,j,k=0,ret=0;
        do
        {
                if(v[k]!=w[k])
                        ret=1;
                k+=1;
        }while(k<12 && ret==0);
        return ret;
}


int treenextsl(int *branch, int *v)
	//calcula el nombre de possicions next per un vector, es podria integrar amb treesl, pero em lio amb els punters
	//v es el vector de possició, branch es un vector amb el nombre de possibilitats per casella
{				
        int i,cont=1;
        for(i=0;i<7;i+=2)
	{
                if(v[i]==0)
                        branch[i]=2; 		//o no passa res o entra un cotxe nou
                else
		{
			if(v[nextp[i][0]]==0 && v[mlook[i]]==0)
                       		branch[i]=3; 	   	//cotxe no es mou, cotxe es mou i entra cotxe, cotxe es mou i no entra cotxe
			else
				branch[i]=1;		//no passa res
		}
	}
        for(i=8;i<12;i++)
	{
                if(v[i]==0)
                        branch[i]=1;			//no passa res
                else
		{
			if(v[nextp[i][0]]==0 && v[nextp[i][1]]==0)
                	        branch[i]=3;                 //el coche es queda quiet, es mou recte al centre, o es mou a caselles finals
			else
				branch[i]=1;		//no passa res
		}
	}
        for(i=1;i<8;i+=2)
                if(v[i]==0)
                        branch[i]=1;                 //no passa res
                else
                        branch[i]=2;                 //es mou o no
        for(i=0;i<12;i++)
                cont*=branch[i];
        return cont;
}


void treesl(int *branch, int *v, int **m, int cont)   
	//crea totes les possicions next per un vector a una matriu m
	//rep com a paràmetres el vector de possició, el vector branch de treenextsl,la matriu m i el nombre de vectors next calculat a treenextsl
	//massa variables div, s'ha de revisar i potser fer-la més senzilla
{
        int i, j, div1, div2=cont, div3, div4, mov, z;
        for(i=0;i<cont;i++)
                for(j=0;j<12;j++)
                        m[i][j]=v[j];
	//primer omplim tota la matriu amb el vector del que calculem els next als vectors fila
	//després creem divisions als vectors columna segons el vector branch, les divisions sempre es van acumulant, la divisió a cada vecto columna és el 	       producte de la divisió i-1 pel vector branch i
	//llavors a cada segment de les divisions anem fent el que correspon, moure cotxes (afecta a la casella a on es mou) entrar cotxes o no fer res
        for(i=0;i<7;i+=2)     //possicions inicials
        {
                div1=div2/branch[i];
                div2=div1;
                if(branch[i]==2)			//vol dir que no hi ha cotxe
                {
                        div3=div1;
                        do
                        {
                                div4=div3+div1;
                                for(j=div3;j<div4;j++)
                                        m[j][i]+=1;       //entra un cotxe nou
                                div3=div4+div1;
                        }while(j<cont);
                }
                else				//vol dir que si hi ha cotxe
                {
			if(branch[i]==3)	//vol dir que es pot moure
			{
                        	div3=div1;
                        	do
                        	{
                                	div4=div3+div1;
                                	for(j=div3;j<div4;j++)
                                	{
                                        	mov=nextp[i][0];
                                        	m[j][mov]+=1;     //cotxe es mou i entra cotxe
                                	}
                                	div3=div4+div1;
                                	for(j=div4;j<div3;j++)
                                	{
                                        	m[j][i]-=1;	//cotxe s mou i no entra cotxe
                                        	m[j][mov]+=1;  
                                	}
                                	div3+=div1;
                        	}while(j<cont);
			}
                }
        }
        for(i=8;i<12;i++)			//possicions centrals
        {
                div1=div2/branch[i];
                div2=div1;
                if(branch[i]==3)		//hi ha cotxe i es pot moure
                {
                        div3=div1;
                        do
                        {
                                div4=div3+div1;
                                for(j=div3;j<div4;j++)
                                {
                                        m[j][i]-=1;
                                        mov=nextp[i][0];		//es mou al centre 
                                        m[j][mov]+=1;
                                }
                                div3=div4+div1;
                                for(j=div4;j<div3;j++)
                                {
                                        m[j][i]-=1;
                                        mov=nextp[i][1];		//es mou a caselles finals
                                        m[j][mov]+=1;
                                }
                                div3+=div1;
                        }while(j<cont);
                }
        }
        for(i=1;i<8;i+=2)			//possicions finals
        {
                div1=div2/branch[i];
                div2=div1;
                if(branch[i]==2)			//hi ha cotxe
                {
                       div3=div1;
                       do
                       {
                               div4=div3+div1;
                               for(j=div3;j<div4;j++)
                                       m[j][i]-=1;          //es mou
                               div3=div4+div1;
                       }while(j<cont);
                }

        }
}


int grid(struct position * node, struct position ** crashv, struct position ** nocrashv)
	//crea tots els nodes, així com els punters de tots els vectors next
	//rep com a parámetres els vectors vuits de nodes i punters a nodes crash i nocrash, aquí els omple
{
        int q=0,p=0,i,j,k,crashn=0,nocrashn=0,cont,aux,crash;
	//q es el node del bucle principal, p els nodes ja assignats al estudiar q
	//crashn i nocrasn son el nombre de vectos crash i nocrash
	//cont es el nombre de vectors next retornat per treenextsl
	//aux es un enter auxiliar
	//crash controla si es o no un vector crash
	//i,j,k iteracions
        int *branch, *jprev;
        int **m;
	//matriu de nodes next, es crea i es desassigna dins del bucle principal
        branch=vector(12);
	//branch es el vector de possibilitats calculat a treenextsl 
        jprev=vector(N); 
	//jprev controla el nombre de previs de la possició j 
        node[0].pos=vector(12);
	//estat inicial, el vector 0 (calloc assigna a 0)
        do
	//node principal, s'estudia el node q, q<=p, quan q=p ens aturem, tots els nodes next ja han estat assignats
        {
                crash=0;
                i=0;
                do
                {
                        if(node[q].pos[i]>1)
                                crash=1;
                        i+=1;
                }while(i<12 && crash==0);
                if(crash==1)
		//si es crash, no mirem matriu next, l'afegim als crasv i el marquem iscrash=1
                {
                        crashv[crashn]=&node[q];
                        node[q].iscrash=1;
                        crashn+=1;
                }
                else
		//si no es crash, l'afegim a nocrashv, el marquem com nocrash i calculem la matriu m de vectos next
                {
                        nocrashv[nocrashn]=&node[q];
                        node[q].iscrash=0;
                        nocrashn+=1;
                        cont=treenextsl(branch,node[q].pos);
                        m=matrix(cont,12);
                        treesl(branch,node[q].pos,m,cont);
			node[q].branch=vector(12);
			for(k=0;k<12;k++)
				//afegim el vector de moviments al node
				node[q].branch[k]=branch[k];
                        node[q].next=svectorp(cont);
                        for(i=0;i<cont;i++)
			//a aquest bucle i es cada un dels vectors de la matriu de next m
                        {
                                j=-1;
				//per que surti amb el número correcte          
                                do
                                {
                                        j+=1;
//                                        aux=compare(m[i],node[j].pos);
                                        k=0;   //esborrar des d'aqui per utilitzar compare, va mes lent amb compare
                                        aux=0;
                                        do
                                        {
                                                if(m[i][k]!=node[j].pos[k])
                                                        aux=1;
                                                k+=1;
                                        }while(k<12 && aux==0);
                                }while(j<p && aux==1);
				//p es el nombre d'estructures ja assignades, j surt de'aquí com a p si es nova i sinó com l'estructura que és
                                if(aux==1)
                                {
                                        p+=1;
					//afegim una estructura nova 
                                        node[p].pos=vector(12);
                                        j+=1;
					//fem j=p
                                        for(k=0;k<12;k++)
                                                node[p].pos[k]=m[i][k];   //igualar-ho per punters
					//aquí el vector de possició del node p
                                }
                                node[q].next[i]=&node[j];
				//assignem com a vector next 
                                jprev[j]+=1;
				//sumem un node de previs
                        }
                        freem(m,cont);
                }
                node[q].npos=q;
                node[q].nnext=cont;
		//afegim més informació al node q
                q+=1;
        }while(q<p+1);  //while(q!=p); 
        for(i=0;i<q;i++)	//aixó ho podria fer sense el jprev quan fai jprev[j]+=1 afegir-ho directament a l'estructura j
        {
                aux=jprev[i];
                node[i].nprev=aux;
        }
        free(branch);
        free(jprev);
        return q;
}


int * rotate(int *v, int d)
{
        int i,j,k;
        int *w;
        w=vector(12);
        if(d!=0 && d!=1 && d!=2 && d!=3)
        {
                printf("\nrotació incorrecta!!!!!!");
                exit(1);
        }
        for(i=0;i<7;i+=2)     //con otra numeracion del vector no habría de diferenciar
        {
                j=(i+2*d)%8;
                w[j]=v[i];
		printf("\n%d\t%d\n",i,j);
		printf("\n%d\t%d\n",v[i],w[j]);
                k=i+1;
                j+=1;
                w[j]=v[k];
		printf("\n%d\t%d\n",k,j);
		printf("\n%d\t%d\n",v[k],w[j]);
        }
        for(i=8;i<12;i++)
        {
                j=(i+d)%12;
                if(j<8) 
                        j+=8;
                w[j]=v[i];
		printf("\n%d\t%d\n",i,j);
		printf("\n%d\t%d\n",v[i],w[j]);
        }
        return w;
}

void symmetry(struct position ** nocrash, struct position ** nocrashs)
{
        int i,j=0,k,l,m,aux;
        int * w;
        FILE * pf;
        pf=fopen("compare.dat","w+");
        if(pf==NULL)
        {
                printf("\nno puc obrir el fitxer!!!!!");
                exit(1);
        }
        nocrashs[0]=nocrash[0];
        for(i=1;i<S;i++)
        {
                fprintv(nocrash[i]->pos,12,pf);
                for(l=1;l<4;l++)
                {
                        w=rotate(nocrash[i]->pos,l);
                        for(k=0;k<=j;k++)
                        {
//                                aux=compare(w,nocrashs[k]->pos);
				m=0;   //esborrar des d'aqui per utilitzar compare, va mes lent amb compare
				aux=0;
				do
				{
					if(w[m]!=nocrash[k]->pos[m])
						aux=1;
					m+=1;
				}while(m<12 && aux==0);
                                if(aux==0)
                                        break;
                        }
                        if(aux==0)
                                break;
			free(w);
                }
                if(aux==1)
                {
                        j+=1;
                        nocrashs[j]=nocrash[i];
                }
        }
	printf("\n\n%d",j);
}


int numtran(struct position ** nocrash)
{
	int i,total=0;
	for(i=0;i<S;i++)
		total+=nocrash[i]->nnext;
	return total;
}


void movetran(int * branch, int * move, int next, int nnext)   //es poden arribar a moure 13 caselles, si entra un de fora
{
	int i,j,k=12,aux,segment,pos;
	int * div;
	div=vector(12);			//el vector div s'hauria de passar com a parámetre i calcular-se només una vegada a treenextsl
	for(i=0;i<16;i++)
		move[i]=i;
	div[0]=branch[0];
	segment=nnext/div[0];
	pos=next/segment;
	aux=pos%branch[0];
	if(branch[0]==2)		//es pot fer amb l'aux aquest if?
	{
		if(aux==1)
			move[k]=0;
	}
	else
	{
		if(aux>0)
		{
			move[0]=nextp[0][0];
			if(aux==1)
				move[k]=0;
		}
	}
	for(i=2;i<7;i+=2)
	{
		j=i-2;
		div[i]=div[j]*branch[i];
		k+=1;	
		segment=nnext/div[i];
		pos=next/segment;
		aux=pos%branch[i];
		if(branch[i]==2)
		{
			if(aux==1)
				move[k]=i;
		}
		else
		{
			if(aux>0)
			{
				move[i]=nextp[i][0];
				if(aux==1)
					move[k]=i;
			}
		}
	}
	div[8]=div[6]*branch[8];
	segment=nnext/div[8];
	pos=next/segment;
	aux=pos%branch[8];
	if(branch[8]==3)
	{
		if(aux==1)
			move[8]=nextp[8][0];
		else if(aux==2)
			move[8]=nextp[8][1];
	}
	for(i=9;i<12;i++)
	{
		j=i-1;
		div[i]=div[j]*branch[i];
		segment=nnext/div[i];
		pos=next/segment;
		aux=pos%branch[i];
		if(branch[i]==3)
		{
			if(aux==1)
				move[i]=nextp[i][0];
			else if(aux==2)
				move[i]=nextp[i][1];
		}
	}	
	div[1]=div[11]*branch[1];
	k=16;
	segment=nnext/div[1];
	pos=next/segment;
	aux=pos%branch[1];
	if(branch[1]==2)
		if(aux==1)
			move[1]=k;
	for(i=3;i<8;i+=2)
	{
		j=i-2;
		div[i]=div[j]*branch[i];
		k+=1;	
		segment=nnext/div[i];
		pos=next/segment;
		aux=pos%branch[i];
		if(branch[i]==2)
			if(aux==1)
				move[i]=k;
	}
	free(div);
}

/*
int movecount(struct transition vtran)
{
	int i,nmove=0;
	for(i=0;i<16;i++)
	{
		if(i!=vtran.move[i])
		{
			vtran.vmove[i]=1;
			nmove+=1;
		}
	}
	return nmove;
}*/


int * crashcount(struct transition vtran)
{
	int i;
	int * ncrash;
	ncrash=vector(4);
	for(i=0;i<7;i+=2)
		if(vtran.next->pos[i]>1)
		{
			ncrash[0]+=1;
			ncrash[1]+=1;
		}
	for(i=8;i<12;i++)
		if(vtran.next->pos[i]>1)
		{
			ncrash[0]+=1;
			ncrash[2]+=1;
		}
	for(i=1;i<8;i+=2)
		if(vtran.next->pos[i]>1)
		{
			ncrash[0]+=1;
			ncrash[3]+=1;
		}
	return ncrash;
}


void filltran(struct position ** nocrash, struct transition * vtran)
{
	int i,j,nnext,total=-1;
	for(i=0;i<S;i++)
	{
		nnext=nocrash[i]->nnext;
		for(j=0;j<nnext;j++)
		{
			total+=1;
			vtran[total].prev=nocrash[i];
			vtran[total].next=nocrash[i]->next[j];
			vtran[total].move=vector(16);
			movetran(vtran[total].prev->branch,vtran[total].move,j,nnext);
			vtran[total].iscrash=vtran[total].next->iscrash;
//			vtran[total].vmove=vector(16);
//			vtran[total].nmove=movecount(vtran[total]);
			vtran[total].ncrash=crashcount(vtran[total]);
		}
	}
}


int numcol(struct transition *vtran, int ntran)
{
	int i,ncol=0;
	for(i=0;i<ntran;i++)
		ncol+=vtran[i].ncrash[0];
	return ncol;
}


void fillcol(struct transition * vtran, struct collision * vcol, int ntran)
{
	int i,j,k,ncol=0,aux, aux2;
	for(i=0;i<ntran;i++)
	{
		if(vtran[i].iscrash==1)
		//si hi han colisions
		{
			for(j=0;j<12;j++)
			//j indica a on són
			{
				if(vtran[i].next->pos[j]>1)
				//si hi ha accident a j
				{
					aux=0;
					k=0;
					vcol[ncol].tran=&vtran[i];
					vcol[ncol].pos=j;
					vcol[ncol].ncar=vtran[i].next->pos[j];
					vcol[ncol].isempty=vtran[i].prev->pos[j];
					if(vcol[ncol].isempty==1)
					{
						if(vtran[i].move[j]==j)
							vcol[ncol].ismove=0;
						else
							vcol[ncol].ismove=1;
					}
					else
						vcol[ncol].ismove=-111;
					vcol[ncol].pprev=vector(2);
					vcol[ncol].pprev[0]=-111;
					vcol[ncol].pprev[1]=-111;
					vcol[ncol].look=vector(2);
					vcol[ncol].look[0]=-111;
					vcol[ncol].look[1]=-111;
					do  //podria definir una matriu de moviments marxa enrere
					{
						if(vtran[i].move[k]==j)
						//un dels cotxers implicats ve de la possició k
						{
							if(k<8)
							//el cotxe que es mou no és del centre
							{
								vcol[ncol].pprev[0]=k;
								vcol[ncol].look[0]=mlook[k];
							}
							else
							{
								vcol[ncol].pprev[1]=k;
								vcol[ncol].look[1]=mlook[k];
							}	
							aux+=1;
						}
						k+=1;
					}while(aux<vcol[ncol].ncar);
					//sortim quan ja hem hem revisat totes les colisions	
					ncol+=1;
				}
			}
		}
	}
}


void printfilesl(int nnode, struct position * node, FILE *pf, FILE *qf)
//printa informacio a fitxer
{
        int i,j,k,crasht=0;
        int *v, *w, *z, *x;
        int **m;
        v=vector(17);
        w=vector(13);
	z=vector(16);
	x=vector(12);
        m=matrix(3,13);
	for(i=0;i<16;i++)
		z[i]=i;
        for(i=0;i<nnode;i++)
        {
                fprintv(node[i].pos,12,qf);
//                fprintf(qf,"\t%d",node[i].npos);
                if(node[i].iscrash==0)
                {
                        fprintv(node[i].pos,12,pf);
/*                        fprintf(pf,"\t%d",node[i].npos);
                        fprintf(pf,"\t%d",node[i].nnext);
                        fprintf(pf,"\t%d",node[i].nprev);
                        fprintf(pf,"\t%d",node[i].nextcrash);*/
                }
        }
    /*    for(i=0;i<ntran/2;i++)
        {
                fprintv(vtran[i].prev->pos,12,rf);
                fprintv(vtran[i].next->pos,12,rf);
                fprintv(z,16,rf);
                fprintv(vtran[i].move,16,rf);
                fprintf(rf,"\n");
        }
        for(i=ntran/2;i<ntran;i++)
        {
                fprintv(vtran[i].prev->pos,12,tf);
                fprintv(vtran[i].next->pos,12,tf);
                fprintv(z,16,tf);
                fprintv(vtran[i].move,16,tf);
                fprintf(rf,"\n");
        }
        fprintf(sf,"%d nodes\n",nnode);
        fprintf(sf,"%d nodes no crash\n",S);
        fprintf(sf,"%d nodes crash\n",nnode-S);
        fprintf(sf,"%d transicions\n",ntran);
        for(i=0;i<ntran;i++)
        {
                if(vtran[i].iscrash==1)
                        crasht+=1;
                v[vtran[i].nmove]+=1;
                w[vtran[i].ncrash[0]]+=1;
                for(j=1;j<4;j++)
                {
                        k=j-1;
                        m[k][vtran[i].ncrash[j]]+=1;
                }
        }
        fprintf(sf,"%d transicions no crash\n",ntran-crasht);
        fprintf(sf,"%d transicions crash\n",crasht);
        for(i=0;i<17;i++)
                fprintf(sf,"%d transicions amb %d moviments\n",v[i],i);
        for(i=0;i<13;i++)
                fprintf(sf,"%d transicions amb %d accidents\n",w[i],i);
        for(i=0;i<13;i++)
                fprintf(sf,"%d transicions amb %d accidents a l'inici\n",m[0][i],i);
        for(i=0;i<13;i++)
                fprintf(sf,"%d transicions amb %d accidents al mig\n",m[1][i],i);
        for(i=0;i<13;i++)
                fprintf(sf,"%d transicions amb %d accidents al final\n",m[2][i],i);
	for(i=0;i<ncol;i++)
	{
	}*/
        free(v);
	free(w);
	free(z);
        freem(m,3);
}


void clascol(int ncol, struct collision * vcol, FILE * cf)
//calculo tot dues vegades, una pel número i l'altre pel vector de punters
{
	int i, j, k, emptyn=0, noemptyn=0, startn=0, middlen=0, endn=0, emptymiddlen=0, empty2n=0, emptyleftn=0, emptyrightn=0;
	struct collision **empty, **noempty, **emptymiddle, **empty2, **emptyleft, **emptyright;;
	for(i=0;i<ncol;i++)
	{
		if(vcol[i].isempty==0)
			emptyn+=1;
		else
			noemptyn+=1;
	}
	printf("\n\n\nTenim %d colisions %d estava buit %d no estaba buit\n\n\n",ncol,emptyn,noemptyn);
	fprintf(cf,"\n\n\nTenim %d colisions %d estava buit %d no estaba buit\n\n\n",ncol,emptyn,noemptyn);
	empty=scvectorp(emptyn);
	noempty=scvectorp(noemptyn);
	j=0;
	k=0;
	for(i=0;i<ncol;i++)
	{
		if(vcol[i].isempty==0)
		{
			empty[j]=&vcol[i];
			j+=1;
		}
		else
		{
			noempty[k]=&vcol[i];
			k+=1;
		}
	}
/*	for(i=0;i<ncol;i++)
	{
		if(vcol[i].pos>7)
			middlen+=1;
		else
		{
			if(vcol[i].pos%2==0)
				startn+=1;
			else
				endn+=1;
		}
	}
	printf("\n\n\nTenemos %d colisiones al inicio %d colisiones al medio %d colisiones al final",start,middle,end);*/
	for(i=0;i<emptyn;i++)
	{
		if(empty[i]->pos>7)
			emptymiddlen+=1;
	}
	printf("\n\n\nTenim %d colisions amb la casella de colisió buida al mig\n\n\n",emptymiddlen);
	fprintf(cf,"\n\n\nTenim %d colisions amb la casella de colisió buida al mig\n\n\n",emptymiddlen);
	emptymiddle=scvectorp(emptymiddlen);
	j=0;
	for(i=0;i<emptyn;i++)
	{
		if(empty[i]->pos>7)
		{
			emptymiddle[j]=empty[i];
			j+=1;
		}
	}
	for(i=0;i<emptyn;i++)
	{
		if(empty[i]->ncar=2)
			empty2n+=1;
	}
	printf("\n\n\nTenim %d colisions amb la casella de colisió buida i 2 cotxes implicats\n\n\n",empty2n);
	fprintf(cf,"\n\n\nTenim %d colisions amb la casella de colisió buida i 2 cotxes implicats\n\n\n",empty2n);
	empty2=scvectorp(empty2n);
	j=0;
	for(i=0;i<emptyn;i++)
	{
		if(empty[i]->ncar=2)
		{
			empty2[j]=empty[i];
			j+=1;
		}
	}
	for(i=0;i<emptyn;i++)
	//dono per fet que la colisio es al centre, amb 2 cotxes, els resultats anteriors ho corrovoren
	{
		if(empty[i]->look[0]!=-111)
			emptyleftn+=1;
		if(empty[i]->look[1]!=-111)
			emptyrightn+=1;
	}
	printf("\n\n\nTenim %d colisions amb la casella de colisio buida i amb el cotxe a la sortida amb un cotxe a la seva esquerra\n",emptyleftn);
	printf("\n\n\nTenim %d colisions amb la casella de colisio buida i amb el cotxe al mig  amb un cotxe a la seva dreta\n\n\n",emptyrightn);
	fprintf(cf,"\n\n\nTenim %d colisions amb la casella de colisio buida i amb el cotxe a la sortida amb un cotxe a la seva esquerra\n",emptyleftn);
	fprintf(cf,"\n\n\nTenim %d colisions amb la casella de colisio buida i amb el cotxe al mig  amb un cotxe a la seva dreta\n\n\n",emptyrightn);
	emptyleft=scvectorp(emptyleftn);
	emptyright=scvectorp(emptyrightn);
	j=0;
	k=0;
	for(i=0;i<emptyn;i++)
	{
		if(empty[i]->look[0]!=-111)
		{
			emptyleft[j]=empty[i];
			j+=1;
		}
		if(empty[i]->look[1]!=-111)
		{
			emptyright[k]=empty[i];
			k+=1;
		}
	}
}
