/* Taylor Artunian
 * ENGR476
 * 10/29/20
 *
 * Implementation of the Dijkstra algorithm for determining the shortest path 
 * between two nodes of a weighted graph.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include <sys/queue.h>

#define		MAX_NODES				                  10

const	char*	FILEPATH                      =	"dat/network_matrix";
const	int	  MAX_SOURCELINE_SIZE		        =	256;
const	char	SOURCE_DELIM[]			          =	" !@#$%^&()_+=-<>?/.,";
      int 	matrix[MAX_NODES][MAX_NODES];
      int	  numNodes;
      FILE*	sourceFile;
      int	  rowCount                      = 0;
      int   colCount                      = 0;

/* Enum and function for return codes and error messages. */
typedef enum {
	OK,
	BAD_LINK_VALUE,
  BAD_SOURCE_COL,
  BAD_SOURCE_ROW
} RCODE;

char* rcodeMsg(RCODE code) {
  switch(code) {
    case OK:              return "OK";
    case BAD_LINK_VALUE:  return "BAD LINK VALUE";
    case BAD_SOURCE_COL:  return "BAD SOURCE COL";
    case BAD_SOURCE_ROW:  return "BAD SOURCE ROW";
    default:              return "UNKNOWN RCODE";
  }
}

typedef struct Link Link;
typedef struct Node Node;

struct Link {
	Node*	destination;
	int	value;
};

struct Node {
	char value;
	Link links[MAX_NODES];
	int	numNeighbors;
};

Node	nodes[MAX_NODES];

RCODE processSourceLine(const char[MAX_SOURCELINE_SIZE]);
RCODE storeMatrixValue(int,int,char*);

/* Parse source file line by line. */
RCODE parseSourceFile(FILE* file) {
  char buffer[MAX_SOURCELINE_SIZE+2];
  while(fgets(buffer, MAX_SOURCELINE_SIZE, file)) {
    RCODE code = processSourceLine(buffer);
    if(code != OK) {
      return code;
    }
    rowCount++;
  }
  return OK;
}

/* Parse source line into Node with links to other nodes. */
RCODE processSourceLine(const char sourceLine[MAX_SOURCELINE_SIZE]) {
  char _sourceLine[MAX_SOURCELINE_SIZE];
  strncpy(_sourceLine, sourceLine, MAX_SOURCELINE_SIZE);

  char* savePointer;
  int col = 0;
  char* c = strtok_r(_sourceLine, SOURCE_DELIM, &savePointer);
  RCODE code;
  do {
    if(rowCount > 0 && col >= colCount) {
      return BAD_SOURCE_COL;
    }
    code = storeMatrixValue(rowCount, col, c);
    if(code == OK) {
      c = strtok_r(0, SOURCE_DELIM, &savePointer);
      col++;
    } else {
      return code;
    }
  } while(c);
  
  if(rowCount == 0) {
    colCount = col;
  }

  return OK;

}

/* Store the value into the matrix and Node array. */
RCODE storeMatrixValue(int row, int col, char* value) {

  /* Parse the link value. '*' indicates no link and gets value of INT_MAX. */
  int linkValue = (*value == '*') ? INT_MAX : atoi(value);
  matrix[row][col] = linkValue;

  Node* src = &nodes[row];
  // if(!src) {
  //   src = nodes[row] = malloc(sizeof(Node));
  // }

  Node* dst = &nodes[col];
  // if(!dst)  {
  //   dst = nodes[col] =  malloc(sizeof(Node));
  // }

  Link* link = &src->links[col];
  link->destination = dst;
  link->value = linkValue;

  //printf("Link Created: (%14p) to (%14p) with value %3i\n", src, dst, linkValue);

  return OK;
}

bool visited[MAX_NODES];
int shortestDistance[MAX_NODES];
Node* previousNode[MAX_NODES];

void setupDijkstra() {
  for(int i=0; i<MAX_NODES; i++) {
    visited[i] = false;
    shortestDistance[i] = INT_MAX;
  }
  shortestDistance[0] = 0;
}

void visit(Node* src, Node* dst) {

  printf("Visiting (%p) en route to (%p)\n", src, dst);

  int neighbors = 0;
  for(int i=0; i<MAX_NODES; i++) {
    Link* link = &src->links[i];
    printf("Link from %14p to %14p with value %i\n", src, link->destination, link->value);
    if(link->destination == NULL || link->value == INT_MAX || link->destination == src) {
      continue;
    }
    neighbors++;
  }

  printf("(%p) has %i neighbors\n\n", src, neighbors);

  int srcIndex = (src - &nodes[0]);
  int neighborIndex;
  Link* link;

  /* Placeholder for each link to neighbor of src. */
  Link shortestLink;
  shortestLink.destination = NULL;
  shortestLink.value = INT_MAX;

  /* Iterate through neighbor links. */
  for(int i=0; i<MAX_NODES; i++) {
    link = &src->links[i];

    printf("Looking at link: (%14p) to (%14p) with value %i\n", src, link->destination, link->value);

    /* Skip NULL link or non-link. */
    if(link->destination == NULL || link->value == INT_MAX) {
        printf("Skipping invalid link.\n");
      continue;
    }

    if(link->destination == src) {
      printf("Skipping loopback link.\n");
      continue;
    }

    neighborIndex = (link->destination - &nodes[0]);

    //Skip previously visited node
    if(visited[neighborIndex] == true) {
      printf("Skipping previously visited node.\n");
      continue;
    }

    /* Update neighbor distances */
    int totalPathDistance = shortestDistance[srcIndex] + link->value;
    printf("Known distance to %14p is %i.\n", link->destination, shortestDistance[neighborIndex]);
    printf("Distance with this link is %i which is less. Updating table.\n", totalPathDistance);
    if(totalPathDistance < shortestDistance[neighborIndex]) {
      shortestDistance[neighborIndex] = totalPathDistance;
      previousNode[neighborIndex] = src;
    }

    //Store the shortest link
    if(link->value < shortestLink.value) {
      shortestLink.destination = link->destination;
      shortestLink.value = link->value;
    }

  }

  /* End if no unvisited destination was found. */
  if(!shortestLink.destination) {
    int dstIndex = dst - &nodes[0];
    int shortestDistanceToDst = shortestDistance[dstIndex];
    printf("Shortest distance from %p to %p is %i.\n", src, dst, shortestDistanceToDst);

    printf("Shortest path (reverse order):\n");
    Node* hop = &nodes[dstIndex];
    Node* prev;
    do {
      printf("  Hop: %p\n", hop);
      prev = previousNode[hop-&nodes[0]];
    } while(hop = prev);

    return;
  }

  printf("Shortest link was to %14p\n", shortestLink.destination);

  /* Update and move on */
  visited[srcIndex] = true;
  printf("Set visited[%i] to true for %p\n\n", srcIndex, src);
  visit(shortestLink.destination, dst);

}

void printDijkstraTable() {
  printf("%16s | %16s | %16s |\n", "Node", "Distance", "Previous");
  for(int i = 0; i < MAX_NODES; i++) {
    printf("%16p | %16i | %16p\n", &nodes[i], shortestDistance[i], previousNode[i]);
  }
}

int main() {
  sourceFile = fopen(FILEPATH, "rb");
  RCODE code = parseSourceFile(sourceFile);
  printf("%s\n", rcodeMsg(code));
  fclose(sourceFile);

  setupDijkstra();
  visit(&nodes[0], &nodes[3]);
  printDijkstraTable();

  return 0;
}
