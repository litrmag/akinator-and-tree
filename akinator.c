#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#define YELLOW "\033[33m"
#define PINK "\033[38;5;206m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define CYAN "\033[36m"
#define RESET "\033[0m"
#define MAGENTA "\033[95m"

typedef char* Elem_t;

struct TreeNode_t
{
    Elem_t data;
    TreeNode_t* left;
    TreeNode_t* right;
    TreeNode_t* parent;
};

enum TypeMode
{
    GUESSING = 1,
    DATABASE = 2,
    SUBSCRIPTION = 3,
    DIFFERENCE = 4
};

enum Direction
{
    LEFT = 1,
    RIGHT = 2
};

void StartAkinator ();

void Guessing ();
void GuessNode (TreeNode_t* node);

void Database ();
void TreeDump (TreeNode_t* root);
int NodeDump (TreeNode_t* root, int* index, FILE* tree_dump);
char* GenerateColor (int index);

void Subscription ();
TreeNode_t* Search (TreeNode_t* root, char* answer, Direction* path, int h);
void PrintPath (TreeNode_t* root, Direction* path, int h);

void Difference ();

char* ReadingLineForGame ();

char* ReadingFromFile ();
TreeNode_t* ParseTree (char* buffer, int* index);
char* ReadingLineForPreorder (char* buffer, int* index);

TreeNode_t* NewNode (Elem_t data);
bool DeleteNode (TreeNode_t* node);
bool LinkLeft (TreeNode_t* child, TreeNode_t* parent);
bool LinkRight (TreeNode_t* child, TreeNode_t* parent);

void Preorder (FILE* file, TreeNode_t* root);

void ExpandSize (char** str, size_t* size_str);

int main ()
{
    StartAkinator ();
    return 0;
}

void StartAkinator ()
{
    printf (YELLOW  "Vyberite rezhim: \n"
                    "1) ugadyvanie\n"
                    "2) pokaz bazy dannyh\n"
                    "3) put do elementa\n"
                    "4) sravneniye elementov\n" RESET);
    TypeMode mode;
    scanf ("%d", &mode);
    while(getchar() != '\n');
    switch (mode)
    {
        case GUESSING :
                Guessing ();
                break;
        case DATABASE :
                Database ();
                break;
        case SUBSCRIPTION :
                Subscription ();
                break;
        case DIFFERENCE :
                Difference ();
                break;
        default :
            printf (RED "Vy vvely chto-to ne to. Poprobuyte eschyo raz\n" RESET);
            while(getchar() != '\n');
            StartAkinator ();
    }
    printf (PINK "Horoshego dnya!\n" RESET);
}

void Guessing ()
{
    char* buffer = ReadingFromFile ();
    int ind = 0;
    TreeNode_t* root = ParseTree (buffer, &ind);
    GuessNode (root);
    const char* filename = "akinator.txt";
    FILE* file = fopen (filename, "w");
    Preorder (file, root);
    fclose (file);
}

void GuessNode (TreeNode_t* root)
{
    int ch = 0;
    if (!root->left)
    {
        printf (CYAN "Eto %s?" RESET " (Vvedite \"d\", esli \"da\" ili \"n\", esli \"net\"): ", root->data);
        if ((ch = getchar()) == 'n')
        {
            while(getchar() != '\n');
            printf (PINK "Vvedite svoi otvet: " RESET);
            char* answer = ReadingLineForGame();
            printf (PINK "Chem <%s> otlichaetsya ot <%s>? " RESET, answer, root->data);
            char* question = ReadingLineForGame();
            TreeNode_t* parent_for_new_question = root->parent;
            TreeNode_t* new_question = NewNode (question);

            if (root->parent->left == root) {
                root->parent->left = NULL;
                root->parent = NULL;
                LinkLeft (new_question, parent_for_new_question);
            } else if (root->parent->right == root) {
                root->parent->right = NULL;
                root->parent = NULL;
                LinkRight (new_question, parent_for_new_question);
            }
            TreeNode_t* new_answer = NewNode (answer);
            LinkLeft (new_answer, new_question);
            LinkRight (root, new_question);
        } else {
            printf (GREEN "Ura, pobeda\n" RESET);
        }
        return;
    }
    printf (MAGENTA "%s" RESET " (Vvedite \"d\", esli \"da\" ili \"n\", esli \"net\"): ", root->data);
    ch = getchar();
    while(getchar() != '\n');
    if (ch == 'd') {
        GuessNode (root->left);
    } else if (ch == 'n') {
        GuessNode (root->right);
    } else {
        printf (RED "Poprobuyte eschyo raz\n" RESET);
        GuessNode (root);
    }
}

void Database ()
{
    char* buffer = ReadingFromFile ();
    int ind = 0;
    TreeNode_t* root = ParseTree (buffer, &ind);
    TreeDump (root);
    printf (CYAN "Mozhete posmotret bazu v faile \"akinator.png\"\n" RESET);
}

void TreeDump (TreeNode_t* root)
{
    FILE* akinator_dump = fopen ("akinator_dump.txt", "w");
    fprintf (akinator_dump, "digraph G{\nrankdir = TB;\n");
    fprintf (akinator_dump, "node[fontsize = 14];\n");
    int ind = 0;
    NodeDump (root, &ind, akinator_dump);
    fprintf (akinator_dump, "}");
    fclose (akinator_dump);
    system ("dot -Tpng akinator_dump.txt -o akinator.png");

}

int NodeDump (TreeNode_t* root, int* index, FILE* tree_dump)
{
    int node_id = *index;
    char* color_data = GenerateColor (*index);
    /*char* color_parent = GenerateColor (root->parent);
    char* color_left_child = GenerateColor (root->left);
    char* color_right_child = GenerateColor (root->right);*/
    fprintf (tree_dump, "%d[shape = none, label =<"
                        "<table border = \"0\" cellborder = \"1\" cellspacing = \"0\">"
                        "<tr>"
                        "<td colspan=\"2\" bgcolor = \"%s\">%s</td>"
                        "</tr>\n",
                        node_id, color_data, root->data);
    /*if (root->parent != NULL)
        fprintf (tree_dump,
        "<tr>"
        "<td colspan=\"2\" bgcolor = \"%s\"> parent: %p</td>"
        "</tr>\n",
        color_parent, root->parent);
    if (root->left != NULL)
        fprintf (tree_dump,
        "<tr>"
        "<td bgcolor = \"%s\"> left_child: %p</td>"
        "<td bgcolor = \"%s\"> right_child: %p</td>"
        "</tr>",
        color_left_child, root->left, color_right_child, root->right);*/
    fprintf(tree_dump, "</table>>];\n");
    (*index)++;
    if (root->left) {
        int left_node_id = NodeDump(root->left, index, tree_dump);
        fprintf (tree_dump, "%d->%d [color = \"green\"];\n", node_id, left_node_id);
    }
    if (root->right) {
        int right_node_id = NodeDump(root->right, index, tree_dump);
        fprintf (tree_dump, "%d->%d [color = \"red\"];\n", node_id, right_node_id);
    }
    return node_id;
}

char* GenerateColor (int index)
{
    srand(index);
    double h = (double)(rand () % 360 + 130) / 360;
    double s = 0.5;
    double v = 1;
    char *color = (char*)calloc(64, sizeof(char));
    sprintf (color, "%.3g %.3g %.3g", h, s, v);
    return color;
}

void Subscription ()
{
    char* buffer = ReadingFromFile ();
    int ind = 0;
    TreeNode_t* root = ParseTree (buffer, &ind);
    printf ("Vvedite uprazhneniye: ");
    char* answer = ReadingLineForGame ();
    Direction* path = (Direction*)calloc(100, sizeof(Direction));
    int h = 0;
    TreeNode_t* found_node = Search (root, answer, path, h);
    if (found_node == NULL) {
        printf (RED "Takogo elementa net. Poprobuyte eschyo raz\n" RESET);
        Subscription ();
        return;
    }
    PrintPath (root, path, h);
}

TreeNode_t* Search (TreeNode_t* root, char* answer, Direction* path, int h)
{
    if (!root)
        return NULL;
    if (strcmp (root->data, answer) == 0)
        return root;
    path[h] = LEFT;
    TreeNode_t* left_node = Search (root->left, answer, path, h + 1);
    if (left_node != NULL)
        return left_node;
    path[h] = RIGHT;
    TreeNode_t* right_node = Search (root->right, answer, path, h + 1);
    if (right_node != NULL)
        return right_node;
    return NULL;
}

void PrintPath (TreeNode_t* root, Direction* path, int h)
{
    if (root == NULL)
        return;
    printf (MAGENTA "%s" RESET, root->data);
    if (path[h] == LEFT) {
        if (root->left != NULL)
            printf (" (da)");
        printf ("\n");
        PrintPath (root->left, path, h + 1);
    } else if (path[h] == RIGHT) {
        if (root->right != NULL)
            printf (" (net)");
        printf ("\n");
        PrintPath (root->right, path, h + 1);
    }
    return;
}

void Difference ()
{
    char* buffer = ReadingFromFile ();
    int ind = 0;
    TreeNode_t* root = ParseTree (buffer, &ind);
    printf ("Vvedite uprazhneniye 1: ");
    char* answer_1 = ReadingLineForGame ();
    Direction* path_1 = (Direction*)calloc(100, sizeof(Direction));
    int h = 0;
    TreeNode_t* found_node_1 = Search (root, answer_1, path_1, h);
    if (found_node_1 == NULL) {
        printf (RED "Takogo elementa net. Poprobuyte eschyo raz\n" RESET);
        Difference ();
        return;
    }
    printf ("Vvedite uprazhneniye 2: ");
    char* answer_2 = ReadingLineForGame ();
    Direction* path_2 = (Direction*)calloc(100, sizeof(Direction));
    TreeNode_t* found_node_2 = Search (root, answer_2, path_2, h);
    if (found_node_2 == NULL) {
        printf (RED "Takogo elementa net. Poprobuyte eschyo raz\n" RESET);
        Difference ();
        return;
    }
    for (int i = 0; i < 100; i++)
    {
        if (path_1[i] == path_2[i]) {
            if (path_1[i] == LEFT)
                root = root->left;
            else
                root = root->right;
        } else {
            printf ("<%s> otlichaetsya ot <%s> v voprose: " MAGENTA "%s\n" RESET, answer_1, answer_2, root->data);
            break;
        }
    }
}

char* ReadingLineForGame ()
{
    size_t size_str = 2;
    char* str = (char*)calloc(size_str, sizeof (char));
    int index_str = 0;
    int ch = 0;
    while ((ch = getchar()) != '\n')
    {
        if (index_str >= size_str)
            ExpandSize (&str, &size_str);
        str[index_str] = ch;
        index_str++;
    }
    str[index_str] = '\0';
    return str;
}

char* ReadingFromFile ()
{
    const char* filename = "akinator.txt";
    FILE* file = fopen (filename, "r");
    if (!file) {
        printf (RED "Error while opening file for reading\n" RESET);
    }
    struct stat file_stat;
    stat (filename, &file_stat);
    char* buffer = (char*)calloc(file_stat.st_size + 1, sizeof (char));
    if (!buffer) {
        printf (RED "Error in allocating memory for writing strings to the buffer\n" RESET);
        return NULL;
    }

    size_t read_char = fread (buffer, sizeof(char), file_stat.st_size, file);
    if (read_char != file_stat.st_size) {
        printf (RED "Error in reading the file\n" RESET);
        return NULL;
    }

    if (fclose (file) == EOF) {
        printf (RED "Error in closing the file\n" RESET);
    }

    return buffer;
}

TreeNode_t* ParseTree (char* buffer, int* index)
{
    if (buffer[*index] != '(') {
        printf (RED "Error in read\n" RESET);
        exit (-1);
    }
    (*index)++;
    char* str = ReadingLineForPreorder (buffer, index);
    TreeNode_t* node = NewNode (str);

    if (buffer[*index] == ')') {
        (*index)++;
        return node;
    } else {
        TreeNode_t* child_left = ParseTree (buffer, index);
        LinkLeft (child_left, node);
        TreeNode_t* child_right = ParseTree (buffer, index);
        LinkRight (child_right, node);
    }
    (*index)++;
    return node;
}

char* ReadingLineForPreorder (char* buffer, int* index)
{
    size_t size_str = 2;
    char* str = (char*)calloc(size_str, sizeof (char));
    int index_str = 0;
    while (buffer[*index] != '(' && buffer[*index] != ')')
    {
        if (index_str >= size_str)
            ExpandSize (&str, &size_str);
        str[index_str] = buffer[(*index)++];
        index_str++;
    }
    str[index_str] = '\0';
    return str;
}

TreeNode_t* NewNode (Elem_t data)
{
    TreeNode_t* node = (TreeNode_t*)calloc(1, sizeof (TreeNode_t));
    if (node == NULL) {
        printf (RED "Memory allocation error\n" RESET);
        exit (1);
    }
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    return node;
}

bool DeleteNode (TreeNode_t* node)
{
    assert (node);

    node->data = NULL;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    return true;
}

bool LinkLeft (TreeNode_t* child, TreeNode_t* parent)
{
    assert (child);
    assert (parent);

    if (parent->left != NULL) {
        printf ("Already has a left child\n");
        return false;
    }
    parent->left = child;
    child->parent = parent;
    return true;
}

bool LinkRight (TreeNode_t* child, TreeNode_t* parent)
{
    assert (child);
    assert (parent);

    if (parent->right != NULL) {
        printf ("Already has a right child\n");
        return false;
    }
    parent->right = child;
    child->parent = parent;
    return true;
}

void Preorder (FILE* file, TreeNode_t* root)
{
    fprintf(file, "(%s", root->data);
    if (root->left) Preorder(file, root->left);
    if (root->right) Preorder(file, root->right);
    fprintf(file, ")");
}

void ExpandSize (char** str, size_t* size_str)
{
    size_t new_size_str = *size_str * 2;
    char* new_str = (char*)realloc(*str, new_size_str * sizeof(char));
    if (new_str != NULL) {
        *str = new_str;
        *size_str = new_size_str;
        return;
    } else {
        printf (RED "Allocation memory error\n" RESET);
        exit (1);
    }
    return;
}
