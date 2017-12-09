#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

struct score_node {
    char name[10];
    int score;
    struct score_node *next;
    struct score_node *prev;
};

void push_score(struct score_node** head_ref, char new_name[10], int new_score)
{
    /*allocate the node */
    struct score_node * new_node = (struct score_node*) malloc(sizeof(struct score_node));
    
    /* put data into name and score */
    strcpy(new_node->name,new_name);
    new_node->score = new_score;

    /*Make next of new node as head and previous as Null */
    new_node->next = (*head_ref);
    new_node->prev = NULL;

    /*change prev of head node to new node
    if statement checks if there is no head ie the list is empty */
    if((*head_ref)!= NULL)
    {
        (*head_ref)->prev = new_node;
    }

    /*move the head to point to the new node */
    (*head_ref) = new_node;
    return;
}

void append_score(struct score_node** head_ref, char new_name[10], int new_score)
{
    /*allocate the node */
    struct score_node * new_node = (struct score_node*) malloc(sizeof(struct score_node));

    /*set a pointer to the head to enable later traversal of the list */
    struct score_node *last = *head_ref; 
    /*put data into name and score */
    strcpy(new_node->name,new_name);
    new_node->score = new_score;

    /*make next of new_node NULL */
    new_node->next = NULL;

    /*if the list is empty make the new node the head */
    if((*head_ref)==NULL)
    {
        new_node->prev = NULL;
        *head_ref = new_node;
        return;
    }

    /* else traverse till node->next ==NULL  ie the last node */
    while(last->next!=NULL)
    {
        last = last->next;
    }

    /* assign the last->next to new_node; */
    last->next = new_node;

    /* make last node the prev of new_node; */
    new_node->prev = last;
    return;
}

void insert_score(struct score_node* prev_node, char new_name[10], int new_score)
{
    /*ensure prev_node isn't NULL */
    if(prev_node == NULL)
    {
        return;
    }

    /*allocate new node */
    struct score_node * new_node = (struct score_node*) malloc(sizeof(struct score_node));

    /* assign values to new node */
    strcpy(new_node->name,new_name);
    new_node->score = new_score;

    /*set new_node->prev to prev_node
    |prev| -> |prev + 1|
          \
           |new| */
    new_node->prev = prev_node;

    /*set new_node's next node to the prev_node's next
     |prev| <-> |prev + 1|
          \   /
          |new| */
    new_node->next = prev_node->next;
    
    /*set prev_node's next to point to new_node
     |prev| <- |prev + 1|
          \    /
          |new| */
    prev_node->next = new_node;

    /*set |prev + 1| to point to new_node if not end of list */
    if(new_node->next != NULL)
    {
        new_node->next->prev = new_node;
    }
    return;
}

void* find_node_on_score(struct score_node ** head_ref, int search_score)
{
    printf("Attempting to Resort\n");
    struct score_node *last = (*head_ref);
    while((last->score)>=search_score)
    {
        if(last->next == NULL){
            return NULL;
        }
        last=last->next;
    }
    printf("Resorting on %d\n",search_score);
    return last;
}

void trim_list(struct score_node ** head_ref)
{
    struct score_node *last = (*head_ref);
    struct score_node * trimmed = NULL;
    int length = 0;
    while(last->next != NULL)
    {
        if(length == 20)
        {
            trimmed = last->next;
            last->next = NULL;
            free(trimmed);
        }
        last = last->next;
        length += 1;
    }
}   

void send_list(struct score_node * node, int comm_fd)
{
    char str[10];
    struct score_node* last = node;
    while (last != NULL)
    {
        write(comm_fd, last->name, strlen(last->name)+1);
        printf("Sending Name:%s\n",last->name);
        sprintf(str, "%d", last->score);
        write(comm_fd, str, strlen(str)+1);
        printf("SendingScore:%s\n",str);
        last = last->next;
    }
}
        

int main(){
	char auth_str[50] = "Please Read\n";
        char control_str[50];
        char * start_send = "Sending Data";
        char * end_send = "That's All";
        char name[50];
        char scorer[10];
        char write_str[50] = "Please Write\n";
	int listen_fd,comm_fd, score;
        FILE * masterboard;
	struct sockaddr_in servaddr;
        struct score_node* master_score = NULL;
        struct loc_node;
        
        masterboard = fopen("scores.board","r");
        while(fgets(name, 10, masterboard)!=NULL){
            name[strcspn(name,"\n")] = 0;
            if(fgets(scorer,10,masterboard)!=NULL){
                score = atoi(scorer);
                append_score(&master_score, name, score);
                printf("Read in %s %d\n", name, score);
            }
        }
        fclose(masterboard);
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	bzero( &servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(1345);

	bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	listen(listen_fd, 10);

	comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);

	while(1){
            bzero (control_str, 50);
	    read(comm_fd, control_str, 50);
            if(strcmp(control_str, auth_str)==0)
            {
                write(comm_fd, start_send, strlen(start_send)+1);
                send_list(master_score,comm_fd);
                /*while(master_score->next!=NULL)
                {
                    write(comm_fd, master_score->name, strlen(master_score->name)+1);
                    write(comm_fd, itoa(master_score->score),strlen(itoa(master_score->score))+1);
                } */
                write(comm_fd, end_send, strlen(end_send)+1);
            }
                if(strcmp(control_str,write_str)==0)
                {
                    read(comm_fd,name,10);
                    name[strcspn(name,"\n")] = '\0';
                    read(comm_fd,scorer,10);
                    scorer[strcspn(name,"\n")] = '\0';
                    score = atoi(scorer);
                    insert_score(find_node_on_score(&master_score,score),name,score);
                    trim_list(&master_score);
                }
        }

}

