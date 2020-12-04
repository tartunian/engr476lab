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

#define MAX_NODES 8

const char *FILEPATH = "dat/network_matrix";
const int MAX_SOURCELINE_SIZE = 256;
const char SOURCE_DELIM[] = " !@#$%^&()_+=-<>?/.,";
int matrix[MAX_NODES][MAX_NODES];
int numNodes;
FILE *sourceFile;
int rowCount = 0;
int colCount = 0;

/* Enum and function for return codes and error messages. */
typedef enum
{
  OK,
  BAD_LINK_VALUE,
  BAD_SOURCE_COL,
  BAD_SOURCE_ROW
} RCODE;

char *rcodeMsg(RCODE code)
{
  switch (code)
  {
  case OK:
    return "OK";
  case BAD_LINK_VALUE:
    return "BAD LINK VALUE";
  case BAD_SOURCE_COL:
    return "BAD SOURCE COL";
  case BAD_SOURCE_ROW:
    return "BAD SOURCE ROW";
  default:
    return "UNKNOWN RCODE";
  }
}

typedef struct Link Link;
typedef struct Node Node;

struct Link
{
  Node *destination;
  int value;
};

struct Node
{
  char name;
  Link links[MAX_NODES];
  int numNeighbors;
};

Node nodes[MAX_NODES];

RCODE processSourceLine(const char[MAX_SOURCELINE_SIZE]);
RCODE storeMatrixValue(int, int, char *);
void printDijkstraTable();
void printDijkstraTable();

/* Parse source file line by line. */
RCODE parseSourceFile(FILE *file)
{
  char buffer[MAX_SOURCELINE_SIZE + 2];
  while (fgets(buffer, MAX_SOURCELINE_SIZE, file))
  {
    RCODE code = processSourceLine(buffer);
    if (code != OK)
    {
      return code;
    }
    rowCount++;
  }
  return OK;
}

/* Parse source line into Node with links to other nodes. */
RCODE processSourceLine(const char sourceLine[MAX_SOURCELINE_SIZE])
{
  char _sourceLine[MAX_SOURCELINE_SIZE];
  strncpy(_sourceLine, sourceLine, MAX_SOURCELINE_SIZE);

  char *savePointer;
  int col = 0;
  char *c = strtok_r(_sourceLine, SOURCE_DELIM, &savePointer);
  RCODE code;
  do
  {
    if (rowCount > 0 && col >= colCount)
    {
      return BAD_SOURCE_COL;
    }
    code = storeMatrixValue(rowCount, col, c);
    if (code == OK)
    {
      c = strtok_r(0, SOURCE_DELIM, &savePointer);
      col++;
    }
    else
    {
      return code;
    }
  } while (c);

  if (rowCount == 0)
  {
    colCount = col;
  }

  return OK;
}

/* Store the value into the matrix and Node array. */
RCODE storeMatrixValue(int row, int col, char *value)
{

  /* Parse the link value. '*' indicates no link and gets value of INT_MAX. */
  int linkValue = (*value == '*') ? INT_MAX : atoi(value);
  matrix[row][col] = linkValue;

  Node *src = &nodes[row];
  // if(!src) {
  //   src = nodes[row] = malloc(sizeof(Node));
  // }

  Node *dst = &nodes[col];
  // if(!dst)  {
  //   dst = nodes[col] =  malloc(sizeof(Node));
  // }

  Link *link = &src->links[col];
  link->destination = dst;
  link->value = linkValue;

  //printf("Link Created: (%14p) to (%14p) with value %3i\n", src, dst, linkValue);

  return OK;
}

bool visited[MAX_NODES];
int shortestDistance[MAX_NODES];
Node *previousNode[MAX_NODES];

void setupDijkstra(int startNodeIndex)
{
  for (int i = 0; i < MAX_NODES; i++)
  {
    visited[i] = false;
    shortestDistance[i] = INT_MAX;
    //previousNode[i] = 
  }
  //Loopback distance.
  shortestDistance[startNodeIndex] = 0;
}

int getUnvisitedLinks(Node *src, Link **linksOut)
{
  int srcIndex = (src - &nodes[0]);
  int linkCount = 0;
  for (int i = 1; i < MAX_NODES; i++)
  {
    Link *link = &src->links[i];
    int neighborIndex = link->destination - &nodes[0];
    if (link->destination == NULL || link->value == INT_MAX || link->destination == src || visited[neighborIndex])
    {
      continue;
    }
    linksOut[linkCount] = link;
    linkCount++;
  }
  return linkCount;
}

int compareLinks(const void *a, const void *b)
{
  Link **linkA_p = (Link **)a;
  Link **linkB_p = (Link **)b;

  return (*linkA_p)->value - (*linkB_p)->value;
}

int getSortedUnvisitedLinks(Node *src, Link **linksOut)
{
  int numLinks = getUnvisitedLinks(src, linksOut);
  qsort(linksOut, numLinks, sizeof(Link *), compareLinks);
  return numLinks;
}

Node *getClosestUnvisitedNode(Node *src)
{
  int _shortestDistance = INT_MAX;
  Node *closestNode;
  for (int i = 0; i < MAX_NODES; i++)
  {
    if (visited[i])
    {
      continue;
    }
    if (shortestDistance[i] <= _shortestDistance)
    {
      _shortestDistance = shortestDistance[i];
      closestNode = &nodes[i];
    }
  }
  return closestNode;
}

void visit(Node *src, Node *dst)
{

  //printf("Visiting %c en route to %c\n", src->name, dst->name);

  int srcIndex = (src - &nodes[0]);
  int neighborIndex;
  Link *link;

  /* Get list of links in order of increasing value/cost. */
  Link *sortedLinks[MAX_NODES - 1];
  int numLinks = getSortedUnvisitedLinks(src, sortedLinks);
  //printf("%c has %i unvisited neighbors\n", src->name, numLinks);
  for (int i = 0; i < numLinks; i++)
  {
    //printf("%c ", sortedLinks[i]->destination->name);
  }
  //printf("\n");

  /* Update distances to neighbors */
  for (int i = 0; i < numLinks; i++)
  {
    link = sortedLinks[i];
    neighborIndex = (link->destination - &nodes[0]);

    //printf("Looking at link: %c to %c with value %i\n", src->name, link->destination->name, link->value);

    int totalPathDistance = shortestDistance[srcIndex] == INT_MAX ? link->value : shortestDistance[srcIndex] + link->value;
    //printf("Known distance to %c is %i.\n", link->destination->name, shortestDistance[neighborIndex]);
    if (totalPathDistance < shortestDistance[neighborIndex])
    {
      //printf("Distance with this link is %i which is less. Updating table.\n", totalPathDistance);
      shortestDistance[neighborIndex] = totalPathDistance;
      previousNode[neighborIndex] = src;
    }
  }

  /* End if no unvisited destination was found. */
  if (numLinks == 0)
  {
    return;
  }

  /* Update and move on */
  visited[srcIndex] = true;

  Node *nextNode = getClosestUnvisitedNode(src);
  if (nextNode)
  {
    visit(nextNode, dst);
  }
  else
  {
    return;
  }
}

void printDijkstraTable()
{
  printf("%16s | %16s | %16s |\n", "Destination", "Distance", "Previous");
  for (int i = 0; i < colCount; i++)
  {
    printf("%16c | %16i | %16c\n", nodes[i].name, shortestDistance[i], previousNode[i] == NULL ? '-' : previousNode[i]->name);
  }
}

void printPath(Node *src, Node *dst)
{
  int srcIndex = src - &nodes[0];
  int dstIndex = dst - &nodes[0];
  Node *nextNode[MAX_NODES];

  /* Unroll te previousNode array which is the path but in reverse. */
  Node *hop = &nodes[dstIndex];
  Node *prev;
  do
  {
    if(hop - &nodes[0] == 0) {
      break;
    }
    prev = previousNode[hop - &nodes[0]];
    nextNode[prev - &nodes[0]] = hop;
  } while (hop = prev);

  /* Unroll the nextNode array which is the path. */
  hop = &nodes[srcIndex];
  Node *next;
  do
  {
    printf("%c", hop->name);
    if(hop - &nodes[0] == dstIndex) {
      break;
    }
    printf("->");
    next = nextNode[hop - &nodes[0]];
  } while (hop = next);
  printf("\n");
}

void setupNodes()
{
  //Give nodes a name from A-Z and set default link value to INT_MAX
  for (int i = 0; i < MAX_NODES; i++)
  {
    Node *node = &nodes[i];
    node->name = 65 + i;
    for (int j = 0; j < MAX_NODES; j++)
    {
      Link *link = &node->links[j];
      link->value = INT_MAX;
    }
  }
}

int main()
{

  char filename[256];

  printf("Please enter path to matrix file (blank = default matrix): ");
  fgets(filename, 256, stdin);
  filename[strlen(filename) - 1] = '\0';
  printf("\n");

  if (filename[0] == '\0')
  {
    sprintf(filename, "%s", FILEPATH);
  }

  setupNodes();
  
  /* Open and parse the matrix file. */
  sourceFile = fopen(filename, "rb");
  RCODE code = parseSourceFile(sourceFile);
  fclose(sourceFile);

  while (true)
  {

    char startNodeName;
    char endNodeName;
    int startNodeIndex;
    int endNodeIndex;
    Node *startNode;
    Node *endNode;

    printf("Start node: ");
    startNodeName = getchar();
    getchar();
    startNodeIndex = startNodeName - 65;
    startNode = &nodes[startNodeIndex];

    printf("End node: ");
    endNodeName = getchar();
    getchar();
    endNodeIndex = endNodeName - 65;
    endNode = &nodes[endNodeIndex];

    setupDijkstra(startNodeIndex);
    visit(startNode, endNode);
    printf("Shortest distance from %c to %c is %d.\n", startNodeName, endNodeName, shortestDistance[endNodeIndex]);
    printPath(startNode, endNode);
    printf("\n");
  }

  return 0;
}
