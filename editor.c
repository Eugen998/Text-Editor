#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct list {
	char content;
	struct list* next;
	struct list* prev;
}*List;

typedef struct cursor {
	int l;
	int c;
}*Cursor;

typedef struct command{
	char s[20];
	struct command* next;
	struct command* prev;
}*Command;

typedef struct copy{
	struct copy* next;
	struct copy* prev;
	struct list* l;
	int line;
	int ch;
}*Copy;

List createList(char c)
{
	List l=malloc(sizeof(struct list));
	l->content=c;
	l->next=NULL;
	l->prev=NULL;
	return l;
}


Command createCommand(char c[20]);
Command add_to_end_command(Command C, char c[20]);
Command readCommands(Command C,int *ok);
List executeCommands(Command C,List l,Cursor k,Copy *x,const char *file,int *count);
void displayList(List l);
List add_to_end_list(List l, char c);
List add_inside_list(List l, List ins, Cursor k);
List deleteLast(List l);
List delete(List l,Cursor k,int n);
List backspace(List l,Cursor k);
List read(List l,List ins,Cursor ,Copy *x);
List deleteLine(List l, Cursor k, int n);
Cursor initCursor(Cursor k);
int countLines(List l);
int countLetters(List l, int n);
void deleteList(List l);
void deleteCommands(Command C);
Copy createCopy(List l,Cursor k);
List mirror(List a,List b);
Copy addCopy(Copy x, List l, Cursor k);
List reload(List l, Copy x, Cursor k);
void printCopy(Copy x);
void deleteCopy(Copy x);
List copy(List l, List b);

int main(int argc, char *argv[])
{
	int ok=0,count=0,nr=0;
	List l=NULL;
	Copy x=NULL;
	Command C=NULL;
	Cursor k=NULL;
	List ins=NULL;
	k=initCursor(k);
	for(;;)
	{
		nr++;
		l=read(l,ins,k,&x);
		if(nr==1) x=addCopy(x,l,k);
		C=readCommands(C,&ok);
		l=executeCommands(C,l,k,&x,argv[1],&count);
		if(ok==1) 
		{
			printf("%d %d\n",k->l,k->c);
			displayList(l);
			deleteList(l);
			free(k);
			deleteCommands(C);
			deleteList(ins);
			return 0;
		}
		C=NULL;
	}
	return 0;
}

Copy createCopy(List l,Cursor k)
{
	Copy x=malloc(sizeof(struct copy));
	x->line=k->l;
	x->ch=k->c;
	x->next=NULL;
	x->prev=NULL;
	x->l=mirror(x->l,l);
	return x;
}
List copy(List l, List b)
{
	if(b==NULL) return NULL;
	l->content=b->content;
	if(l->next!=NULL) deleteList(l->next);
	b=b->next;
	while(b!=NULL)
	{
		l=add_to_end_list(l,b->content);
	}
	return l;
}
List mirror(List a, List b)
{
	if(b==NULL) return NULL;
	else
	{
		List temp=b;
		a=createList(temp->content);
		temp=temp->next;
		while(temp!=NULL)
		{
			add_to_end_list(a,temp->content);
			temp=temp->next;
		}
	}
	return a;
}

Copy addCopy(Copy x, List l, Cursor k)
{
	if(x==NULL) 
		{
			x=createCopy(l,k);
			return x;
		}
	Copy new=createCopy(l,k);
	Copy temp=x;
	while(temp->next!=NULL)
		temp=temp->next;
	new->prev=temp;
	temp->next=new;
	return x;
}

Cursor initCursor(Cursor k)
{
	k=malloc(sizeof(struct cursor));
	k->l=-1;
	k->c=-1;
	return k;
}

Command createCommand(char c[20])
{
	Command C=malloc(sizeof(struct command));
	strcpy(C->s,c);
	C->next=NULL;
	C->prev=NULL;
	return C;
}

Command add_to_end_command(Command C, char c[20])
{
	Command temp=C;
	Command new = createCommand(c);
	if(C==NULL) return new;
	while(temp->next != NULL)
		temp=temp->next;
	new->next=NULL;
	new->prev=temp;
	temp->next=new;
	return C;
}

Command readCommands(Command C,int *ok)
{
	char c[20];
	fgets(c,20,stdin);
	if(strcmp(c,"::i")==0)
	{ 
		C=NULL;
		return C;
	}
	while(strcmp(c,"::i\n")!=0)
	{
		if(strcmp(c,"q\n")==0) 
		{
			*ok=1;
			return C;
		}
		C = add_to_end_command(C,c);
		fgets(c,20,stdin);
	}
	return C;
}
List go_to_cursor(List l,Cursor k)
{
	List aux=NULL;
	if(k->l==1 && k->c==1) return l;
	List temp=l;
	int line=1,character=1;
	while(line < k->l)
	{
		if(temp->content == '\n') 
		{
			line++;
		}
		aux=temp;
		temp=temp->next;
	}
	while(character < k->c)
	{
		aux=temp;
		temp=temp->next;
		character++;

	}
	temp=aux;
	return temp;
}
List backspace(List l, Cursor k)
{
	int nr;
	if(k->c==1 && k->l==1) return l;
	List temp=l;
	List aux=NULL;
	temp=go_to_cursor(l,k);
	if(temp->content=='\n') 
	{
		nr=1;
		k->l--;
		aux=temp->prev;
		while(aux != NULL && aux->content !='\n')
		{
			aux=aux->prev;
			nr++;
		}
		k->c=nr+1;
	}
	if(temp->prev != NULL)
	{
		temp->next->prev=temp->prev;
		temp->prev->next=temp->next;
		k->c--;
	}
	else
	{
		l=temp->next;
		k->c--;
	}
	return l;
}

List delete(List l,Cursor k,int n)
{
	List aux=NULL;
	List temp=NULL;
	int nr=0;
	if(k->l==-1 && k->c==-1) return l;
	while(nr<n)
	{
		if(k->l==1 && k->c==1) aux=l;
		else 
		{
			temp=go_to_cursor(l,k);
			aux=temp->next;
		}
		if(aux->next==NULL) return l;
		if(aux->next->next==NULL)
		{
			aux->next=NULL;
			return l;
		}
		else
		{
			aux->next->next->prev=aux;
			aux->next=aux->next->next;
			nr++;
		}
	}
	return l;
}
int countLines(List l)
{
	if(l==NULL) return 0;
	List temp=l;
	int nr=1;
	while(temp->next!=NULL)
	{
		if(temp->content=='\n') nr++;
		temp=temp->next;
	}
	return nr;
}
List deleteLine(List l,Cursor k,int n)
{
	List temp=NULL;
	List aux=NULL;
	int nr=countLines(l);
	if(n==0) 
	{
		if(k->l==-1&&k->c==-1)
		{
			k->l=nr;
			n=nr;
		}
		else n=k->l;
		k->c=1;
	}
	if(n==1)
	{
		if(nr==1) return NULL;
		else
		{
			temp=l;
			while(temp->content!='\n') temp=temp->next;
			l=temp->next;
			k->l=1;
			k->c=1;
		}
	}
	else if(n==nr)
	{
		k->l=nr;
		k->c=1;
		temp=go_to_cursor(l,k);
		temp->next=NULL;
		k->c=-1;
		k->l=-1;
	}
	else 
	{
		k->l=n;
		k->c=1;
		temp=go_to_cursor(l,k);
		aux=temp;
		temp=temp->next;
		while(temp->content!='\n')
			temp=temp->next;
		temp=temp->next;
		aux->next=temp;
		temp->prev=aux;
		k->c=1;
		k->l=n;
	}
	return l;
}

List reload(List l, Copy x, Cursor k)
{
	if(x->next==NULL)
	{
		k->l=x->line;
		k->c=x->ch;
		l=x->l;
		deleteList(x->l);
		return NULL;
	}
	Copy aux=x;
	while(aux->next!=NULL) aux=aux->next;
	k->l=aux->line;
	k->c=aux->ch;
	l=aux->l;
	aux->prev->next=NULL;
	return l;
}
void printCopy(Copy x)
{
	printf("Printez back up uri\n");
	int nr=0;
	if(x==NULL) printf("Nu exista back up\n");
	else
	{
		Copy temp=x;
		while(temp!=NULL)
		{
			printf(".");
			nr++;
			displayList(temp->l);
			temp=temp->next;
		}
	}
	printf("%d\n", nr );
}
List executeCommands(Command C,List l,Cursor k,Copy *x,const char *file,int *count)
{
	int i;
	char aux[10]={0};
	if(C==NULL) return l;
	Command temp=C;
	while(temp!=NULL)
	{
		if(temp->s[0]=='u')
		{
			if(temp->next==NULL) 
			{
				if(*x!=NULL) 
				{
					l=reload(l,*x,k);
					return l;
				}
			}
			if(temp->next->s[0]=='r')
			{
				if(temp->next->next==NULL)
				{
					return l;
				}
				else temp=temp->next;
			}
			if(*x!=NULL) 
				{
					l=reload(l,*x,k);
				}	
		}
		if(temp->s[0]=='s') 
			{
				*count=-1;
				FILE *f=fopen(file,"w");
				List temp=l;
				while(temp != NULL)
				{
					fprintf(f,"%c",temp->content);
					temp=temp->next;
				}	
				fclose(f);			
			}
		if(temp->s[0]=='b') 
		{
			*x=addCopy(*x,l,k);
			if(k->l==-1 && k->c==-1)
				l=deleteLast(l);
			else l=backspace(l,k);
		}
		if(temp->s[0]=='d')
		{
			*x=addCopy(*x,l,k);
			int n=0;
			if(temp->s[1]=='l')
			{
				if(temp->s[2]=='\n') n=0;
				else
				{
					strcpy(aux,temp->s+2);
					n=atoi(aux);
				}
				l=deleteLine(l,k,n);
			}
			if(temp->s[1]==' ')
			{
				n=atoi(temp->s+2);
				l=delete(l,k,n);
			}
			if(temp->s[1]=='\n') l=delete(l,k,1);
		}
		if(temp->s[0]=='g'&&temp->s[1]=='l')
		{
			*x=addCopy(*x,l,k);
			strcpy(aux,temp->s+3);
			int n=atoi(aux);
			k->l=n;
			k->c=1;
		}
		if(temp->s[0]=='g'&&temp->s[1]=='c')
		{
			*x=addCopy(*x,l,k);
			char a[2];
			int nrc=0,nrl=0;
			i=3;
			while(temp->s[i]!=' ' && temp->s[i]!='\n')
			{
				a[0]=temp->s[i];
				nrc=nrc*10+atoi(a);
				i++;
			}
			k->c=nrc;
			if(temp->s[i]=='\n')
			{
				if(k->l==-1) k->l=countLines(l);
			}
			else
			{
				i++;
				while(temp->s[i]!='\n')
				{
					a[0]=temp->s[i];
					nrl=nrl*10+atoi(a);
					i++;
				}
				k->l=nrl;
			}	
		}
		(*count)++;
		if(*count==5)
		{
			*count=0;
			FILE *f=fopen(file,"w");
			List temp=l;
			while(temp != NULL)
			{
				fprintf(f,"%c",temp->content);
				temp=temp->next;
			}	
			fclose(f);				
		}
		temp=temp->next;
	}
	return l;
}

void displayList(List l)
{
	List temp=l;
	while(temp != NULL)
	{
		printf("%c",temp->content);
		temp=temp->next;
	}
}
int countLetters(List l, int n)
{
	List temp=l;
	int nr=0;
	int line=1;
	while(line < n)
	{
		if(temp->content == '\n') 
		{
			line++;
		}
		temp=temp->next;
	}
	while(temp->content!='\n' )
	{
		if(temp->next==NULL) 
		{
			nr++;
			break;
		}
		else
		{
			nr++;
			temp=temp->next;
		}
	}
	return nr;
}
List add_to_end_list(List l, char c)
{
	List new = createList(c);
	if(l == NULL) return new;
	List temp=l;
	while(temp->next!=NULL)
		temp=temp->next;
	temp->next = new;
	new->prev = temp;
	new->next = NULL;
	return l;
}

List add_inside_list(List l, List ins, Cursor k)
{
	int nr2=countLines(ins);
	List aux=ins;
	List temp=go_to_cursor(l,k);
	if(aux==NULL) return l;
	while(aux->next!=NULL)
	{
		aux=aux->next;
	}
	if(k->l==1 && k->c ==1)
	{
		temp->prev=aux;
		aux->next=temp;
		l=ins;
		k->l=2;
		k->c=1;
		return l;
	}
	if(temp->next->next==NULL)
	{
		if(temp->next->content=='\n')
		{
			temp->next=NULL;
			ins=add_to_end_list(ins,'\n');
			aux=aux->next;
			aux->next=NULL;
			temp->next=ins;
			ins->prev=temp;
			k->l=k->l+nr2;
			k->c=1;
		}
		else
		{
			temp=temp->next;
			temp->next=ins;
			ins->prev=temp;
			k->l=-1;
			k->c=-1;
		}
	}
	else
	{
		if(temp->next->content=='\n')
		{
			temp->next=temp->next->next;
			ins=add_to_end_list(ins,'\n');
			aux=aux->next;
			aux->next=temp->next;
			temp->next=ins;
			ins->prev=temp;
		}
		else
		{
			ins->prev=temp;
			aux->next=temp->next;
			temp->next->prev=aux;
			temp->next=ins;
		}
		if(nr2>1) k->l=k->l+nr2;
		else k->l++;
		k->c=1;
	}
	return l;	
}

List deleteLast(List l)
{
	List temp=l;
	while(temp->next!=NULL)
		temp=temp->next;
	temp->prev->next=NULL;
	temp->prev=NULL;
	return l;
}

List read(List l,List ins,Cursor k,Copy *x)
{
	if(l!=NULL) *x=addCopy(*x,l,k);
	ins=NULL;
	char c,s;
	while((c=getchar()))
	{
		if(c==':')
		{
			s=getchar();
			if(s==':') 
			{
				getchar();
				getchar();
				break;
			}
			else 
			{
				if(k->l==-1 && k->c==-1)
				{
					l=add_to_end_list(l,c);
					l=add_to_end_list(l,s);
				}
				else 
				{
					ins=add_to_end_list(ins,c);
					ins=add_to_end_list(ins,s);
				}
			}
		}
		else 
		{
			if(k->l==-1 && k->c == -1) l=add_to_end_list(l,c);
			else ins=add_to_end_list(ins,c);
		}
	}
	if(k->l!=-1 && k->l!=-1) l=add_inside_list(l,ins,k);
	return l;
}

void deleteList(List l)
{
	List temp=NULL;
	while(l!=NULL)
	{
		temp=l;
		l=(l)->next;
		free(temp);
	}
	free(l);
}
void deleteCommands(Command C)
{
	Command temp=NULL;
	while(C!=NULL)
	{
		temp=C;
		C=(C)->next;
		free(temp);
	}
	free(C);
}
void deleteCopy(Copy x)
{
	Copy temp=NULL;
	while(x!=NULL)
	{
		temp=x;
		x=(x)->next;
		deleteList(temp->l);
		free(temp);
	}
}