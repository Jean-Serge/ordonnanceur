#include <stdio.h>
#include <stdlib.h>
#include "contexte.h"
#include "hw.h"

/* struct ctx_s ctx_ping;  */
/* struct ctx_s ctx_pong;  */

struct ctx_s *premier = NULL;
struct ctx_s *courant = NULL; 
 
/**
 *  Fonction de test pour vérifier le fonctionnement 
 *  de dump_sp()
 */
void toto()
{
  int toto;
  dump_sp();
  printf("%x\n",(int) &toto);
  
}

int termine()
{
	return premier == NULL;
}
int create_ctx(int stack_size, func_t f, void *args)
{
  struct ctx_s *ctx = (struct ctx_s*) malloc(sizeof(struct ctx_s));
  
  if(premier == NULL)
    {
      premier = ctx;
      courant = ctx;
      ctx->next = ctx;
    }
  else
    {
      courant->next = ctx;
      courant = ctx;
      ctx->next = premier;
    }
  
  return init_ctx(ctx, stack_size, f, args);
}

int init_ctx(struct ctx_s *ctx, int stack_size, func_t f, void *args)
{
  ctx->stack = (char *) malloc(stack_size);
  if(ctx->stack == NULL)
    perror("Problème d'allocation mémoire.");
  /* ebp et esp sont stockés à la fin de la pile */
  ctx->ctx_esp = ctx->ctx_ebp = ctx->stack+stack_size-sizeof(int);
  ctx->etat = E_READY;
  ctx->f = f;
  ctx->ctx_magic = CTX_MAGIC;
  ctx->args = args;

  return ctx->etat;
}

void start_sched()
{
  start_hw();
  setup_irq(TIMER_IRQ, yield);
}

void yield()
{
  printf("Je yield.\n");
  if(courant != NULL && courant->next != NULL){
    switch_to_ctx(courant->next);
  }
}

void switch_to_ctx(struct ctx_s *ctx)
{
  struct ctx_s *tmp;

  /* 
     On utilise irq_disable() dans le switch pour 
     éviter que 2 contextes cherchent à utiliser
     la même ressource en même temps 
  */
  switch(ctx->etat)
    {
    case E_TERMINATED:
      irq_disable();
      tmp = premier;    

      /* On supprime de la liste le contexte terminée */
      while(1)
      	{
	  if(tmp == NULL)
	    break;
	if(tmp == premier) 
	{
		premier = NULL;
	  	break;
	}
      	  if(tmp->next == ctx)
      	    {
	      tmp->next = ctx->next;
      	      free(ctx->stack);
      	      free(ctx);
      	      break;
      	    }
	  tmp = tmp->next;
      	}
      irq_enable();
      break;

    case E_READY:   
      irq_disable();
      ctx->etat = E_ACTIVATED;
      /* Si courant vaut NULL alors c'est qu'il s'agit 
	 du premier contexte à charger */
      if (courant != NULL) {
	save_ctx();
      }
      courant = ctx;
      load_ctx();  

      irq_enable();
      courant->f(courant->args); 
      irq_disable();
      courant->etat = E_TERMINATED;
      
      yield();
      irq_enable();
      break;

    case E_ACTIVATED:
      irq_disable();
      save_ctx()
	courant = ctx;
      load_ctx();
      irq_enable();
      break;
  
    case E_BLOCKED:
      break;
    }
  
}

void sem_down(struct sem_s *sem)
{
  struct ctx_s *tmp = sem->liste;

  if(--(sem->cpt) <= 0)
    {
      courant->etat = E_BLOCKED;
      if(tmp == NULL)
	{
	  sem->liste = courant;
	  courant->sem_next = NULL;
	}
      else
	{
	  do{
	    if(tmp->sem_next == NULL)
	      {
		tmp->sem_next = courant;
		courant->sem_next = NULL;       
		break;
	      }
	  }while((tmp = tmp->sem_next) != NULL);
	}
    }
}

void sem_up(struct sem_s *sem)
{
  struct ctx_s *tmp = sem->liste;
  (sem->cpt)++;
  if(sem->cpt == 0)
    {
      sem->liste->etat = E_ACTIVATED;
      sem->liste->sem_next = NULL;
      sem->liste = NULL;
    }
  else if(sem->cpt < 0)
    {
      sem->liste = tmp->sem_next;
      tmp->sem_next = NULL;
      tmp->etat = E_ACTIVATED;
    }
}

void sem_init(struct sem_s *sem, unsigned int val)
{
  sem->cpt = val;
  sem->liste = NULL;
}


