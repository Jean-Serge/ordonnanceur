#ifndef H_CONTEXTE
#define H_CONTEXTE

#define CTX_MAGIC 0xDEADBEEF

#define N 500000
#define N2 100
/* Macro d'affichage de la pile */
#define dump_sp()							\
  do{									\
     void *esp, *ebp;							\
     asm("movl %%esp, %0" "\n\t"					\
	 "movl %%ebp, %1"						\
	 :"=r" (esp),							\
	   "=r" (ebp));							\
     printf("EBP : %x, ESP : %x\n", (unsigned int)ebp, (unsigned int)esp); \
     }while(0);								\

#define save_ctx()				\
  do{						\
    asm("movl %%esp, %0" "\n\t"			\
	"movl %%ebp, %1"			\
	:"=r"(courant->ctx_esp),		\
	 "=r"(courant->ctx_ebp):);		\
  }while(0);					\

#define load_ctx()				\
  do{						\
    asm("movl %0, %%esp" "\n\t"			\
	"movl %1, %%ebp"			\
	:					\
	:"r"(courant->ctx_esp),			\
	 "r"(courant->ctx_ebp));		\
  }while(0);					\

typedef void (func_t) (void*);
typedef struct object_s{
  int value;
}object_t;
/* Etats de la pile */
enum etat {E_READY, E_ACTIVATED, E_TERMINATED, E_BLOCKED};

/* Structure de notre nouvelle pile (avec liste chainée) */
struct ctx_s
{
  struct ctx_s *next;
  struct ctx_s *sem_next;
  void *ctx_esp, *ctx_ebp;
  func_t *f;
  void * args;
  char *stack;
  unsigned int ctx_magic;
  enum etat etat;
};

struct sem_s{
  int cpt;
  struct ctx_s *liste;
};

extern int termine();
/**
 * Initialise la pile ctx de taille stack_size.
 * Initialise les pointeurs esp et ebp à la fin de la pile.
 * Passe la pile en état E_READY
 */
extern int init_ctx(struct ctx_s *ctx, int stack_size, func_t, void *args);
/**
 * Complète la fonction init_ctx en ajoutant
 * l'allocation dynamique de la liste chainée
 * de contextes.
 */
extern int create_ctx(int, func_t, void*);
/**
 * Enregistre le contexte courant et
 * charge le contexte suivant.
 */ 
extern void switch_to_ctx(struct ctx_s *ctx);
/**
 * Effectue un switch_to_ctx du contexte 
 * suivant dans la liste.
 */
extern void yield();
/** 
 * Installe le gestionnaire d'interruptions
 * et initialise le matériel.
 */ 
extern void start_sched();


extern void sem_up(struct sem_s*);
extern void sem_down(struct sem_s*);

extern void sem_init(struct sem_s *, unsigned int);


#endif
