# search.py
# ---------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
# 
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


"""
In search.py, you will implement generic search algorithms which are called by
Pacman agents (in searchAgents.py).
"""

import util

class SearchProblem:
    """
    This class outlines the structure of a search problem, but doesn't implement
    any of the methods (in object-oriented terminology: an abstract class).

    You do not need to change anything in this class, ever.
    """

    def getStartState(self):
        """
        Returns the start state for the search problem.
        """
        util.raiseNotDefined()

    def isGoalState(self, state):
        """
          state: Search state

        Returns True if and only if the state is a valid goal state.
        """
        util.raiseNotDefined()

    def getSuccessors(self, state):
        """
          state: Search state

        For a given state, this should return a list of triples, (successor,
        action, stepCost), where 'successor' is a successor to the current
        state, 'action' is the action required to get there, and 'stepCost' is
        the incremental cost of expanding to that successor.
        """
        util.raiseNotDefined()

    def getCostOfActions(self, actions):
        """
         actions: A list of actions to take

        This method returns the total cost of a particular sequence of actions.
        The sequence must be composed of legal moves.
        """
        util.raiseNotDefined()


def tinyMazeSearch(problem):
    """
    Returns a sequence of moves that solves tinyMaze.  For any other maze, the
    sequence of moves will be incorrect, so only use this for tinyMaze.
    """
    from game import Directions
    s = Directions.SOUTH
    w = Directions.WEST
    return  [s, s, w, s, w, w, s, w]

def depthFirstSearch(problem: SearchProblem):
    """
    Search the deepest nodes in the search tree first.

    Your search algorithm needs to return a list of actions that reaches the
    goal. Make sure to implement a graph search algorithm.

    To get started, you might want to try some of these simple commands to
    understand the search problem that is being passed in:

    print("Start:", problem.getStartState())
    print("Is the start a goal?", problem.isGoalState(problem.getStartState()))
    print("Start's successors:", problem.getSuccessors(problem.getStartState()))
    """
    "*** YOUR CODE HERE ***"
    
    stack = util.Stack() # stack: ((x,y),[path]) 
    visited = [] # Visited states

    # Check if initial state is goal state #
    if problem.isGoalState(problem.getStartState()):
        return []

    # Start from the beginning and find a solution, path is an empty list #
    stack.push((problem.getStartState(),[]))

    while(not stack.isEmpty()):

        # Get informations of current state #
        current_state,actions = stack.pop() # Take position and path
        visited.append(current_state) # Visit state

        # Terminate condition: reach goal #
        if problem.isGoalState(current_state):
            return actions

        # Get successors of current state #
        successors = problem.getSuccessors(current_state)
        # print(succ)

        for succ in successors:
            # print(succ)
            next_state, action, cost = succ
            
            if next_state not in visited and next_state not in (state[0] for state in stack.list):
                # print(succ[0])
                stack.push((next_state,actions + [action]))

    return []


def breadthFirstSearch(problem: SearchProblem):
    """Search the shallowest nodes in the search tree first."""
    "*** YOUR CODE HERE ***"
    q = util.Queue()
    visited = []

    current_state = problem.getStartState()

    if problem.isGoalState(current_state): return []

    q.push((current_state,[])) # put current state and actions in queue

    while not q.isEmpty():
        current_state, actions = q.pop()
        visited.append(current_state)

        if problem.isGoalState(current_state):
            return actions
        
        successors = problem.getSuccessors(current_state)

        for succ in successors:
            next_state, action, cost = succ
            if next_state not in visited and next_state not in (state[0] for state in q.list):
                q.push((next_state,actions + [action]))
    return []



def uniformCostSearch(problem: SearchProblem):
    """Search the node of least total cost first."""
    "*** YOUR CODE HERE ***"
    util.raiseNotDefined()

def nullHeuristic(state, problem=None):
    """
    A heuristic function estimates the cost from the current state to the nearest
    goal in the provided SearchProblem.  This heuristic is trivial.
    """
    from util import manhattanDistance
    dis = manhattanDistance(state,problem.goal)
    # walls = problem.walls
    # # Calculate number of walls in between 
    # x1,y1 = state
    # x2,y2 = problem.goal
    # wall_count = 0
    # # Calculate on x-axis
    # step_x = 1 if x2 > x1 else -1
    # for x in range(x1, x2 + step_x, step_x):
    #     if walls[x][y1]:  # if wall
    #         wall_count += 1

    # # Calculate on y-axis
    # step_y = 1 if y2 > y1 else -1
    # for y in range(y1 + step_y, y2 + step_y, step_y):
    #     if walls[x2][y]:  # if wall
    #         wall_count += 1
    # dis += wall_count

    return dis

def aStarSearch(problem: SearchProblem, heuristic=nullHeuristic):
    """Search the node that has the lowest combined cost and heuristic first."""
    "*** YOUR CODE HERE ***"

    pq = util.PriorityQueue()
    # print(problem._visited)
    visited = []

    current_state = problem.getStartState()

    if problem.isGoalState(current_state):return []

    pq.push((current_state, [], 0),0) # (state,actions,real cost), priority

    while not pq.isEmpty():
        current_state, actions,current_cost = pq.pop()

        if current_state in visited:continue

        visited.append(current_state)
        # print(current_state)
        if problem.isGoalState(current_state):
            return actions
        
        successors = problem.getSuccessors(current_state)

        for successor, action, step_cost in successors:
            if successor not in visited:
                real_cost = current_cost + step_cost
                h = heuristic(successor,problem)
                # print(h)
                f = real_cost + h
                # print(f,successor)
                pq.update((successor, actions + [action],real_cost), f)

    return []


# Abbreviations
bfs = breadthFirstSearch
dfs = depthFirstSearch
astar = aStarSearch
ucs = uniformCostSearch
