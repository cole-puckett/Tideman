#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
bool check_current_pairs(int i, int j);
void sort_pairs(void);
void swap(int i, int highest, int difference[]);
void lock_pairs(void);
bool create_cycle(int winner, int loser);
bool check_locked(int);
void print_winner(void);
void higher_rank(int ranks[], int column);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // update the ranks of the candidates
    // if the vote does not match any candidates name
    // return false
    for (int i = 0; i < candidate_count; ++i)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // record number of voters who prefer each candidate over every candidate ranked underneath them
    // loop through each candidate to update preferences for each one
    for (int i = 0; i < candidate_count; ++i)
    {
        higher_rank(ranks, i);
    }

    // //*********************************************************************************************************************
    // printf("Preferences: \n");
    // for (int i = 0; i < candidate_count; ++i)
    // {
    //     for (int j = 0; j < candidate_count; ++j)
    //     {
    //         printf("[%i]", preferences[i][j]);
    //     }
    //     printf("\n");
    // }
    return;
}

// loop through each column and update preferences for the given candidate
void higher_rank(int ranks[], int rank)
{
    for (int i = 0; i < candidate_count; ++i)
    {
        if (rank < i)
        {
            preferences[ranks[rank]][ranks[i]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    pair_count = 0;
    // determine the pairs that have winners using the preferences matrix
    // find difference of the preferences[i][j] from preferences[j][i] to see who wins and loses in each pair
    // find differences by looping through each element of the array and comparing
    // add a check so you don't accidentally add the same pair twice
    // add the pairs to the pairs array
    for (int i = 0; i < candidate_count; ++i)
    {
        for (int j = 0; j < candidate_count; ++j)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                if (check_current_pairs(i, j))
                {
                    pairs[pair_count].winner = i;
                    pairs[pair_count].loser = j;
                    ++pair_count;
                }
            }
            else if (preferences[i][j] < preferences[j][i])
            {
                if (check_current_pairs(i, j))
                {
                    pairs[pair_count].winner = j;
                    pairs[pair_count].loser = i;
                    ++pair_count;
                }
            }
        }
    }

    // // *********************************************************************************************************************
    // printf("Pairs: \n");
    // for (int i = 0; i < pair_count; ++i)
    // {
    //     printf("Winner: %i, Loser: %i\n", pairs[i].winner, pairs[i].loser);
    // }
    return;
}

bool check_current_pairs(int i, int j)
{
    // loop through each pair in the array
    // if a pair already existed with the values, return false

    for (int count = 0; count < candidate_count; count++)
    {
        if (pairs[count].winner == i)
        {
            if (pairs[count].loser == j)
            {
                return false;
            }
        }

        if (pairs[count].winner == j)
        {
            if (pairs[count].loser == i)
            {
                return false;
            }
        }
    }
    return true;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // sort the pairs using selection sort depending on which victories are the strongest

    // find the difference of each victory and add values to an array
    // sort the pairs array using the values from the difference array
    int difference[pair_count];
    for (int i = 0; i < pair_count; ++i)
    {
        difference[i] = preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner];
    }

    // // *********************************************************************************************************************
    // printf("Differences: \n");
    // for (int i = 0; i < pair_count; ++i)
    // {
    //     printf("[%i]", difference[i]);
    // }
    // printf("\n");

    int loc;

    for (int i = 0; i < pair_count; ++i)
    {
        int highest = difference[i];
        loc = i;
        for (int j = i; j < pair_count; ++j)
        {
            if (difference[j] > highest)
            {
                highest = difference[j];
                loc = j;
            }
        }
        if (loc != i)
        {
            swap(i, loc, difference);
        }
    }

    // // *********************************************************************************************************************
    // printf("Sorted pairs: \n");
    // for (int i = 0; i < pair_count; ++i)
    // {
    //     printf("Winner: %i, Loser: %i\n", pairs[i].winner, pairs[i].loser);
    // }

    return;
}

void swap(int i, int loc, int difference[])
{
    // swap the current array pair with the pair with the highest difference

    pair swap = pairs[i];
    pairs[i] = pairs[loc];
    pairs[loc] = swap;

    int change = difference[i];
    difference[i] = difference[loc];
    difference[loc] = change;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // lock each pair in decreasing order
    // check for a cycle each time by making sure no one can be preferred over someone who already has a lock (true)
    for (int i = 0; i < pair_count; ++i)
    {
        if (!create_cycle(pairs[i].winner, pairs[i].loser))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }

    // // FOR DEBUGGING - print locked matrix
    // for (int i = 0; i < candidate_count; ++i)
    // {
    //     for (int j = 0; j < candidate_count; ++j)
    //     {
    //         printf("[%d]", locked[i][j]);
    //     }
    //     printf("\n");
    // }
    return;
}

bool create_cycle(int winner, int loser)
{

    // base case
    // if the loser can reach the winner, it would create a cycle
    if (loser == winner)
    {
        return true;
    }

    for (int i = 0; i < pair_count; ++i)
    {
        if (locked[loser][i])
        {
            if(create_cycle(winner, i))
            {
                return true;
            }
        }
    }

    return false;
}

// Print the winner of the election
void print_winner(void)
{
    // find which candidate has no arrows pointing at them in the locked matrix
    // print each candidate that has no one preferred over them
    for (int i = 0; i < candidate_count; ++i)
    {
        if (check_locked(i))
        {
            printf("%s\n", candidates[i]);
        }
    }
    return;
}

bool check_locked(int column)
{
    for (int row = 0; row < candidate_count; ++row)
    {
        if (locked[row][column])
        {
            return false;
        }
    }
    return true;
}
