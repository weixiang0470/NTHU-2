import math
import heapq

# 全域常數：無限大
INF = math.inf

# ------------------------------------------------------
# 1. 工具函式：複製矩陣
# ------------------------------------------------------
def copy_matrix(matrix):
    return [row[:] for row in matrix]

# ------------------------------------------------------
# 2. 工具函式：行列化簡 (Row & Column Reduction)
#    回傳：(化簡後的矩陣, 化簡所需的成本)
# ------------------------------------------------------
def reduce_matrix(matrix):
    n = len(matrix)
    reduced = copy_matrix(matrix)
    cost = 0
    
    # (A) 行化簡
    for i in range(n):
        row_min = min(reduced[i])
        if row_min != INF and row_min > 0:
            cost += row_min
            for j in range(n):
                if reduced[i][j] != INF:
                    reduced[i][j] -= row_min
    # print("test")
    # for row in reduced:
    #     print(row)

    # (B) 列化簡
    for j in range(n):
        col_min = INF
        for i in range(n):
            if reduced[i][j] < col_min:
                col_min = reduced[i][j]
        if col_min != INF and col_min > 0:
            cost += col_min
            for i in range(n):
                if reduced[i][j] != INF:
                    reduced[i][j] -= col_min
    # print(f"col = {cost}")
    return reduced, cost

def graph_to_adjacency_matrix(graph):
    # 獲取所有節點
    nodes = list(graph.keys())
    # 節點數量
    n = len(nodes)
    # 初始化鄰接矩陣，預設值為 0（表示沒有邊）
    adjacency_matrix = [[INF] * n for _ in range(n)]

    # 將節點名稱映射到索引
    node_to_index = {node: i for i, node in enumerate(nodes)}

    # 填充鄰接矩陣
    for node, neighbors in graph.items():
        for neighbor, weight in neighbors.items():
            # 找到節點和鄰居的索引
            i = node_to_index[node]
            j = node_to_index[neighbor]
            # 填入權重
            adjacency_matrix[i][j] = weight

    return adjacency_matrix, nodes

def next_reach_origin(current_node,map):
    # print(current_node,map)
    return True if map[int(current_node[-1])-1][0] >= 0 and map[int(current_node[-1])-1][0] is not INF else False

def get_new_map(map,from_node,to_node):

    new_map = copy_matrix(map)
    
    # update out degree to infinite
    for i in range(len(new_map[from_node])):
        new_map[from_node][i] = INF
    
    # update in degree to infinite
    for row in range(len(new_map)):
        new_map[row][to_node] = INF
    
    return new_map
            
def dfs(cur_map,original_map,now_node,current_nodes,cost,best_path,nodes):
    global best_cost
    if len(current_nodes)==6:
        # print("testing",current_nodes)
        if next_reach_origin(current_nodes[-1],cur_map):
            cost += original_map[now_node][0]
            if cost < best_cost:
                best_cost = cost
                best_path = current_nodes.copy()
                best_path.append("v1")
                print(f"Found solution, cost = {best_cost}, path = {best_path}")
            return
        return
    
    for i in range(len(cur_map[now_node])):
        if cur_map[now_node][i] < INF and nodes[i] not in current_nodes:
            tmp_map, lb = reduce_matrix(cur_map)
            new_cost = cost + original_map[now_node][i] + lb
            # print(original_map[now_node][i],"+",cost)
            # print(f"lb = {lb}")
            if new_cost < best_cost:
                # print(current_nodes,i+1,"best_cost=",best_cost,cost + original_map[now_node][i],lb)
                new_map = get_new_map(tmp_map,now_node,i)
                current_nodes.append(nodes[i])
                print(current_nodes,i+1,"best_cost=",best_cost,cost + original_map[now_node][i],lb)
                # print(f"Current nodes : {current_nodes}, LB = {lb}, current_cost = {cost}")
                dfs(new_map.copy(),original_map,i,current_nodes,cost + original_map[now_node][i],best_path,nodes)
                
                current_nodes.pop()
                # print(current_nodes)
            else: 
                # print(f"{current_nodes}, reject {nodes[i]}")
                continue
        else:
            # print(f"{current_nodes}, {nodes[i]} no path found")
            continue
    return

    
# ------------------------------------------------------
# 6. 主程式：測試範例 (6 個城市：A=0, B=1, C=2, D=3, E=4, F=5)
# ------------------------------------------------------
if __name__ == "__main__":
    # 建構鄰接矩陣
    graph = {
        "v1": {"v2": 1, "v6": 2, "v5": 4},
        "v2": {"v1": 1, "v5": 5, "v4": 8},
        "v3": {"v5": 5, "v6": 6, "v4": 1},
        "v4": {"v2": 8, "v6": 2, "v3": 1},
        "v5": {"v1": 4, "v2": 5, "v6": 3, "v3": 5},
        "v6": {"v1": 2, "v5": 3, "v4": 2, "v3": 6}
    }

    adj_matrix, nodes = graph_to_adjacency_matrix(graph)
    current_nodes=["v1"]
    # tmp_map, lb = reduce_matrix(adj_matrix)
    # cur_map = get_new_map(tmp_map,0,)
    best_cost = INF
    dfs(cur_map=adj_matrix,original_map=adj_matrix,now_node=0,current_nodes=current_nodes,cost=0,best_path=[],nodes=nodes)
    # dfs(adj_matrix,adj_matrix,0,[],INF,0,[])
    # print(next_reach_origin(6,adj_matrix))
    # 輸出結果
    # print("節點順序:", nodes)
    # print("鄰接矩陣:")
    # for row in adj_matrix:
    #     print(row)
        
    # reduced, cost = reduce_matrix(adj_matrix)
    # print(f"cost = {cost}, reduced:")
    # for row in reduced:
    #     print(row)