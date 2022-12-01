#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//link data structure
typedef struct LinkData
{
	int id_node1, id_node2;
	float traverse_cost;
}LinkData;

//graph structure
typedef struct Graph
{
	int nr_nodes, nr_links, nr_warehouses;
	LinkData link;
	float** matrix;
} Graph;

//finds the node with the minimum cost
float minCost(float dist[], int sptSet[], int nr_nodes)
{
	//initialize min value
	int min_index = -1;
	float min = 1000000;
	int i;
	for (i = 0; i < nr_nodes; i++)
		if (sptSet[i] == 0 && dist[i] <= min)
		{
			min = dist[i];
			min_index = i;
		}

	return min_index;
}

//prints what it is requested at Task 1
void printResultTask1(int warehouse, float distance1, float distance2, int way_to_destination[], int nr_stops)
{
	printf("%d\n", warehouse);
	printf("%.1f ", distance1);
	printf("%.1f\n", distance2);
	int i;
	for (i = 0; i < nr_stops - 1; i++)
		printf("%d ", way_to_destination[i]);
	printf("%d", way_to_destination[nr_stops - 1]);
	printf("\n");

}

//implements Dijkstra's algorithm 
void dijkstra(float** matrix, int warehouse, int nr_nodes, int* parent, int store, float* distance, int* nr_stops)
{
	//sptSet[i] will be true if i is included in the shortest path or if shortest distance from src to i is finalized
	int* sptSet = (int*)malloc(sizeof(int) * nr_nodes);
	int i;

	//initialize distance with infinit and sptSet with 0
	for (i = 0; i < nr_nodes; i++)
	{
		distance[i] = 1000000;
		sptSet[i] = 0;
	}

	//distance from warehouse to warehouse will always be 0 
	distance[warehouse] = 0;

	//in perent we will keep the nodes from the shortest path
	for (i = 0; i < nr_nodes; i++)
		parent[i] = -1;

	int count;

	//finding the shortest path for all nodes
	for (count = 0; count < nr_nodes - 1; count++)
	{
		int u = minCost(distance, sptSet, nr_nodes);

		sptSet[u] = 1;
		int v;

		for (v = 0; v < nr_nodes; v++)
			if (!sptSet[v] && matrix[u][v] && matrix[u][v] != 1000000 && distance[u] != 1000000 && distance[u] + matrix[u][v] < distance[v])
			{
				distance[v] = distance[u] + matrix[u][v];
				parent[v] = u;
			}
	}

	//finding the number of nodes that helped biuld the shortest path to the store
	for (i = 0; i < nr_nodes; i++)
		if (parent[i] != -1)
			(*nr_stops)++;

	(*nr_stops)++;

	free(sptSet);
}

//building the road from warehouse to store and back
void constructWayToDestination(int parent[], int store, int* way_to_destination, int* count)
{
	if (parent[store] == -1)
		return;

	constructWayToDestination(parent, parent[store], way_to_destination, count);

	way_to_destination[*count] = store;
	(*count)++;
}

//pushing elements on stack
void stackPush(int stack[], int* size, int item)
{
	stack[*size] = item;
	(*size)++;
}

//popping elements from stack
int stackPop(int stack[], int* size)
{
	if (*size == 0)
		return -1;
	(*size)--;
	return stack[*size];
}

//add element in queue
void queueEnqueue(int queue[], int* queue_len, int item)
{

	int i;
	(*queue_len)++;
	for (i = (*queue_len) - 1; i > 0; i--)
		queue[i] = queue[i - 1];

	queue[0] = item;

}

//add element in queue if it does not exist already
void queueEnqueueIfNotExists(int queue[], int* queue_len, int item)
{

	int i;
	for (i = 0; i < *queue_len; i++)
		if (item == queue[i])
			return;

	(*queue_len)++;
	for (i = (*queue_len) - 1; i > 0; i--)
		queue[i] = queue[i - 1];

	queue[0] = item;

}

//pulls out element from queue
int queueDequeue(int queue[], int* queue_len)
{

	int i;
	if (*queue_len == 0)
		return -1;

	(*queue_len)--;

	int item = queue[*queue_len];
	queue[*queue_len] = -1;
	return item;
}


//get minimum between 2 elements
int getMin(int a, int b)
{
	return (a < b) ? a : b;
}

//find a connected component in a graph
void findComponent(int u, int disc[], int low[], int stack[], int* stack_size, int stkItem[], int nr_nodes, float** matrix, int** found_components, int* nr_components)
{
	static int time = 0;
	disc[u] = low[u] = ++time;

	stackPush(stack, stack_size, u);

	//flag u is in the stack
	stkItem[u] = 1;

	int v;

	for (v = 0; v < nr_nodes; v++)
	{
		if (matrix[u][v])
		{
			if (disc[v] == -1)
			{   //when v is not visited
				findComponent(v, disc, low, stack, stack_size, stkItem, nr_nodes, matrix, found_components, nr_components);
				low[u] = getMin(low[u], low[v]);
			}
			else
				//when v is in the stack, update low for u
				if (stkItem[v])
					low[u] = getMin(low[u], disc[v]);
		}
	}

	int popped_item = 0;

	if (low[u] == disc[u])
	{
		int j = 1;
		while (stack[*stack_size - 1] != u)
		{
			popped_item = stack[*stack_size - 1];
			found_components[*nr_components][j] = popped_item;
			j++;
			//mark as item is popped
			stkItem[popped_item] = 0;
			stackPop(stack, stack_size);
		}
		popped_item = stack[*stack_size - 1];

		//adds component to matrix
		found_components[*nr_components][j] = popped_item;
		stkItem[popped_item] = 0;
		stackPop(stack, stack_size);
		found_components[*nr_components][0] = j;
		(*nr_components)++;
	}
}

//find strong connected components
void strongConComponent(int nr_nodes, float** matrix, int** found_components, int* nr_components)
{
	int* disc = (int*)malloc(sizeof(int) * nr_nodes);
	int* low = (int*)malloc(sizeof(int) * nr_nodes);
	int* stkItem = (int*)malloc(sizeof(int) * nr_nodes);
	int* stack = (int*)malloc(sizeof(int) * nr_nodes);
	int stack_size = 0;
	int i;

	//initialize all elements
	for (i = 0; i < nr_nodes; i++)
	{
		disc[i] = low[i] = -1;
		stkItem[i] = 0;
	}

	//find the connected components
	for (i = 0; i < nr_nodes; i++)
		if (disc[i] == -1)
		{
			findComponent(i, disc, low, stack, &stack_size, stkItem, nr_nodes, matrix, found_components, nr_components);
		}

	free(disc);
	free(low);
	free(stkItem);
	free(stack);
}

//cut all links to the warehouse 
void excludeWarehouse(int nr_nodes, float** matrix, int warehouse)
{
	int i;
	for (i = 0; i < nr_nodes; i++)
	{
		matrix[i][warehouse] = 0;
		matrix[warehouse][i] = 0;
	}
}

//sorts an array
void sortArray(int* arr, int length)
{
	int i, j, aux;
	for (i = 0; i < length - 1; i++)
		for (j = i + 1; j < length; j++)
			if (arr[i] > arr[j])
			{
				aux = arr[i];
				arr[i] = arr[j];
				arr[j] = aux;
			}
}

//creates an array with the sorted positions of the elements from the first column
void sortMatrix(int** matrix, int rows, int* positions)
{
	int i, j, k, aux;
	for (i = 0; i < rows; i++)
		positions[i] = i;
	for (i = 0; i < rows - 1; i++)
		for (j = i + 1; j < rows; j++)
			if (matrix[i][1] > matrix[j][1])
			{
				aux = positions[i];
				positions[i] = positions[j];
				positions[j] = aux;
			}
}

//prints what it is requested at Task 2
void printResultTask2(int** found_components, int nr_components, int* warehouses, int nr_warehouses)
{
	printf("%d\n", nr_components - nr_warehouses);
	int i, j, k = 0;

	//sort the matrix
	for (i = 0; i < nr_components; i++)
		sortArray(found_components[i] + 1, found_components[i][0]);

	int* positions = (int*)malloc(sizeof(int) * nr_components);

	sortMatrix(found_components, nr_components, positions);

	//print the matrix
	for (i = 0; i < nr_components; i++)
	{
		int component_index = positions[i];
		int ok = 1;
		for (j = 0; j < nr_warehouses; j++)
			if (found_components[component_index][1] == warehouses[j])
				ok = 0;
		if (ok == 1)
		{
			for (j = 1; j < found_components[component_index][0]; j++)
				printf("%d ", found_components[component_index][j]);

			printf("%d", found_components[component_index][j]);
			k++;
			if (k < nr_components - nr_warehouses)
				printf("\n");
		}
	}

	free(positions);
}

//check if all nodes have been visited
int visitedAllNodes(int n, int* route, int route_len)
{
	int i, j;
	if (route_len < n)
	{
		return 0;
	}
	for (i = 0; i < n; i++)
	{
		int nodeInRoute = 0;
		for (j = 0; j < route_len; j++)
		{

			if (route[j] == i)
			{
				nodeInRoute = 1;
				break;
			}
		}

		if (!nodeInRoute)
			return 0;
	}
	return 1;
}

//check if we'll go twice through a cycle if we would add the node 'node_to_be_added'
int hasCycle(int* route, int route_len, int node_to_be_added)
{
	int i, j, k;
	//ckeck if we can add the node to the route: - look for a cycle with the popped node
	int o1 = -1; //first occurence
	int cycleDetected = 1;

	if (route_len > 0)
	{
		for (j = route_len - 1; j >= 0; j--)
		{
			//search back for the occurence of the node
			if (route[j] == node_to_be_added)
			{
				o1 = j;
				break;
			}

		}

		int k;

		if (o1 > 0)
		{
			for (i = 0; i < o1; i++)
			{
				cycleDetected = 1;

				if (o1 - i >= route_len - o1)
				{

					//check sub vector
					for (k = 0; k < route_len - o1; k++)
					{
						if (route[i + k] != route[o1 + k])
						{
							cycleDetected = 0;
							break;
						}
					}
				}
				else
				{
					cycleDetected = 0;
				}

				if (cycleDetected == 1)
				{
					break;
				}
			}
		}
		else
		{
			cycleDetected = 0;
		}
	}
	else
	{
		cycleDetected = 0;
	}

	return cycleDetected;
}

//get another route
void populateNextRouteTable(int** prev_route_table, int prev_route_count, int** next_route_table, int* next_route_count, int* queue, int* queue_len, float** matrix, int* nodes, int node_count, int start_node_idx, int** found_routes, int* found_routes_count)
{
	int i, j, k, l;


	int current_queue_len = *queue_len;

	//generate new route table 
	for (j = 0; j < current_queue_len; j++)
	{
		int node_idx = queueDequeue(queue, queue_len);

		for (i = 0; i < prev_route_count; i++)
		{
			if (!matrix[nodes[prev_route_table[i][prev_route_table[i][0]]]][nodes[node_idx]])
				break;
			if (node_idx == start_node_idx && visitedAllNodes(node_count, prev_route_table[i] + 1, prev_route_table[i][0]))
			{
				//we have a complete route
				found_routes[*found_routes_count][0] = prev_route_table[i][0] + 1;

				for (l = 1; l < prev_route_table[i][0] + 1; l++)
					found_routes[*found_routes_count][l] = prev_route_table[i][l];

				found_routes[*found_routes_count][l] = node_idx;
				(*found_routes_count)++;
			}
			else
				if (node_idx != start_node_idx && !hasCycle(prev_route_table[i] + 1, prev_route_table[i][0], node_idx))
				{
					//add successors 
					for (k = 0; k < node_count; k++)
					{
						if (matrix[nodes[node_idx]][nodes[k]] != 0)
						{
							queueEnqueueIfNotExists(queue, queue_len, k);
						}
					}

					//we copy it to the next route table
					//next roue will have prev route len +1
					next_route_table[*next_route_count][0] = prev_route_table[i][0] + 1;

					// copy nodes
					for (l = 1; l < prev_route_table[i][0] + 1; l++)
						next_route_table[*next_route_count][l] = prev_route_table[i][l];

					next_route_table[*next_route_count][l] = node_idx;
					(*next_route_count)++;

				}
		}
	}
}

//determines all possible routes
void findAllRoutes(int node_count, int* nodes, int start_node_idx, float** matrix, int** found_routes, int* found_route_count)
{
	int i, j, k;
	int queue_size = node_count * node_count;
	int queue_len = 0;
	int* queue = (int*)calloc(queue_size, sizeof(int));
	memset(queue, -1, queue_size);

	int** next_route_table;
	int next_route_count = 0;
	float route_cost = 0;
	int route_len = 1;
	int prev_route_table_size = 1;
	int** prev_route_table = (int**)malloc(prev_route_table_size * sizeof(int*));
	prev_route_table[0] = (int*)malloc(node_count * sizeof(int) + 1);
	prev_route_table[0][0] = 1;
	prev_route_table[0][1] = start_node_idx;
	int prev_route_count = 1;


	//enqueue start node successors
	for (i = 0; i < node_count; i++)
	{
		if (matrix[nodes[start_node_idx]][nodes[i]] != 0)
		{
			queueEnqueueIfNotExists(queue, &queue_len, i);
		}
	}

	while (queue_len > 0)
	{
		//next route table size = route count from prev route tabe multiplied with the number of elements from queue
		int next_route_table_size = prev_route_count * queue_len;

		//allocate next route table
		int** next_route_table = (int**)malloc(next_route_table_size * sizeof(int*));
		for (j = 0; j < next_route_table_size; j++)
		{
			next_route_table[j] = (int*)malloc(node_count * node_count * sizeof(int) + 1);
		}

		//reate next route table
		populateNextRouteTable(prev_route_table, prev_route_count, next_route_table, &next_route_count, queue, &queue_len, matrix, nodes, node_count, start_node_idx, found_routes, found_route_count);

		// free prev route table
		for (j = 0; j < prev_route_table_size; j++)
		{
			free(prev_route_table[j]);
		}
		free(prev_route_table);

		//switch route tables
		prev_route_table = next_route_table;
		prev_route_table_size = next_route_table_size;
		prev_route_count = next_route_count;

		next_route_table = NULL;
		next_route_table_size = 0;
		next_route_count = 0;
	}

	// free last route table	
	for (j = 0; j < prev_route_table_size; j++)
	{
		free(prev_route_table[j]);
	}
	free(prev_route_table);
	free(queue);
}


int main()
{
	Graph matrix_data;
	int i, j;
	int* parent;

	//read data and allocate memory 
	scanf("%d", &matrix_data.nr_nodes);
	scanf("%d", &matrix_data.nr_links);
	scanf("%d", &matrix_data.nr_warehouses);

	matrix_data.matrix = (float**)malloc(matrix_data.nr_nodes * sizeof(float*) + 1);

	for (i = 0; i < matrix_data.nr_nodes; i++)
		matrix_data.matrix[i] = (float*)malloc(matrix_data.nr_nodes * sizeof(float) + 1);

	for (i = 0; i < matrix_data.nr_nodes; i++)
		for (j = 0; j < matrix_data.nr_nodes; j++)
			matrix_data.matrix[i][j] = 0;

	for (i = 0; i < matrix_data.nr_links; i++)
	{
		scanf("%d", &matrix_data.link.id_node1);
		scanf("%d", &matrix_data.link.id_node2);
		scanf("%f", &matrix_data.link.traverse_cost);
		matrix_data.matrix[matrix_data.link.id_node1][matrix_data.link.id_node2] = matrix_data.link.traverse_cost;
	}

	int* warehouses = (int*)malloc(sizeof(int) * matrix_data.nr_warehouses);

	for (i = 0; i < matrix_data.nr_warehouses; i++)
		scanf("%d", &warehouses[i]);

	int nr_tasks;
	scanf("%d", &nr_tasks);

	//start doing the tasks
	for (i = 0; i < nr_tasks; i++)
	{
		char task[10];
		scanf("%s", task);

		if (strcmp(task, "e1") == 0)
		{
			int task_warehouse, nr_stores;
			float cost = 0;

			scanf("%d", &task_warehouse);
			scanf("%d", &nr_stores);

			//calculating the shortest path for every store given as input
			for (j = 0; j < nr_stores; j++)
			{
				int store;
				scanf("%d", &store);

				//allocating memory
				int* way_to_destination = (int*)malloc(sizeof(int) * (matrix_data.nr_nodes + 2));
				int count = 0, nr_stops = 0;
				int* parent = (int*)calloc(matrix_data.nr_nodes, sizeof(int));
				float* distance = (float*)malloc(sizeof(float) * matrix_data.nr_nodes);

				//finding the shortest path from warehouse to store
				dijkstra(matrix_data.matrix, task_warehouse, matrix_data.nr_nodes, parent, store, distance, &nr_stops);

				//in way_to_destination we are building the road from warehouse to store and back
				way_to_destination[0] = task_warehouse;
				count++;

				constructWayToDestination(parent, store, way_to_destination, &count);

				//calculating the final cost
				float distance1 = distance[store];
				cost = cost + distance1;
				nr_stops = 0;

				//finding the shortest path from store to warehouse
				dijkstra(matrix_data.matrix, store, matrix_data.nr_nodes, parent, task_warehouse, distance, &nr_stops);

				//calculating the final cost
				float distance2 = distance[task_warehouse];
				cost = cost + distance2;

				constructWayToDestination(parent, task_warehouse, way_to_destination, &count);

				printResultTask1(store, distance1, distance2, way_to_destination, count);

				//free memory
				free(way_to_destination);
				free(parent);
				free(distance);
			}
			printf("%.1f", cost);
		}

		if (strcmp(task, "e2") == 0)
		{
			//allocating memory
			int** found_components = (int**)malloc(matrix_data.nr_nodes * sizeof(int*));
			for (j = 0; j < matrix_data.nr_nodes; j++)
				found_components[j] = (int*)malloc(matrix_data.nr_nodes * sizeof(int) + 1);

			int nr_components = 0;

			//cut ties with the warehouses if more
			for (j = 0; j < matrix_data.nr_warehouses; j++)
				excludeWarehouse(matrix_data.nr_nodes, matrix_data.matrix, warehouses[j]);

			strongConComponent(matrix_data.nr_nodes, matrix_data.matrix, found_components, &nr_components);

			printResultTask2(found_components, nr_components, warehouses, matrix_data.nr_warehouses);

			for (j = 0; j < matrix_data.nr_nodes; j++)
				free(found_components[j]);
			free(found_components);

		}

		if (strcmp(task, "e3") == 0)
		{
			int nr_zones;
			scanf("%d", &nr_zones);
			for (j = 0; j < nr_zones; j++)
			{
				int nr_nodes_zone;
				scanf("%d", &nr_nodes_zone);

				int* zone_nodes = (int*)malloc(sizeof(int) * (nr_nodes_zone + 1));
				int k;

				for (k = 1; k < nr_nodes_zone + 1; k++)
					scanf("%d", &zone_nodes[k]);

				int w;
				float globalMinCost = 100000000;
				for (w = 0; w < matrix_data.nr_warehouses; w++) 
				{
					zone_nodes[0] = warehouses[w];
					//allocate memory
					int found_routes_size = (nr_nodes_zone + 1) * (nr_nodes_zone + 1) * (nr_nodes_zone + 1);
					int** found_routes = (int**)malloc(found_routes_size * sizeof(int*));

					for (k = 0; k < found_routes_size; k++)
						found_routes[k] = (int*)malloc((nr_nodes_zone + 1) * (nr_nodes_zone + 1) * sizeof(int) + 1);

					int found_routes_count = 0;
					findAllRoutes(nr_nodes_zone + 1, zone_nodes, 0, matrix_data.matrix, found_routes, &found_routes_count);

					//find minimum cost route
					int r;
					//this should be float maximum value
					float minRouteCost = 100000000;
					for (k = 0; k < found_routes_count; k++)
					{
						float routeCost = 0;
						// calculate route cost
						for (r = 1; r < found_routes[k][0]; r++)
						{
							routeCost += matrix_data.matrix[zone_nodes[found_routes[k][r]]][zone_nodes[found_routes[k][r + 1]]];
						}
						if (routeCost < minRouteCost)
							minRouteCost = routeCost;
					}
					if (minRouteCost < globalMinCost)
						globalMinCost = minRouteCost;
					

					for (k = 0; k < found_routes_size; k++)
						free(found_routes[k]);
					free(found_routes);

					
				}
				printf("%.1f", globalMinCost);
				if (j < nr_zones - 1)
					printf("\n");
				free(zone_nodes);
			}
		}
		if (i != nr_tasks - 1)
			printf("\n");
	}

	//free memory
	for (i = 0; i < matrix_data.nr_nodes; i++)
		free(matrix_data.matrix[i]);
	free(matrix_data.matrix);
	free(warehouses);

	return 0;
}
