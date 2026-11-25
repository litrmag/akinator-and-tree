#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

typedef int Elem_t;

struct TreeNode_t
{
    Elem_t data;
    TreeNode_t* left;
    TreeNode_t* right;
    TreeNode_t* parent;
};

TreeNode_t* NewNode (Elem_t data);
bool DeleteNode (TreeNode_t* node);
bool LinkLeft (TreeNode_t* child, TreeNode_t* parent);
bool LinkRight (TreeNode_t* child, TreeNode_t* parent);
void Preorder (TreeNode_t* root);
TreeNode_t* ParseTree (char* buffer, int* index);

void TreeDump (TreeNode_t* root);
int NodeDump (TreeNode_t* root, int* index, FILE* tree_dump);
char* GenerateColor (TreeNode_t* node);

int main ()
{
    /*TreeNode_t* node_1 = NewNode (67);
    TreeNode_t* node_2 = NewNode (52, &node_status);
    LinkLeft (node_2, node_1);

    TreeNode_t* node_3 = NewNode (1, &node_status);
    LinkRight (node_3, node_1);

    TreeNode_t* node_4 = NewNode (4, &node_status);
    LinkLeft (node_4, node_2);

    TreeNode_t* node_5 = NewNode (8, &node_status);
    LinkRight (node_5, node_2);

    TreeNode_t* node_6 = NewNode (2, &node_status);
    LinkLeft (node_6, node_3);

    TreeNode_t* node_7 = NewNode (8, &node_status);
    LinkRight (node_7, node_3);

    TreeNode_t* node_8 = NewNode (8, &node_status);
    LinkLeft (node_8, node_4);

    TreeNode_t* node_9 = NewNode (2, &node_status);
    LinkRight (node_9, node_6);

    Preorder (node_1);
    TreeNode_t* node = NewNode(67);
    TreeNode_t* node_ = node;
    for(int i = 0; i < 10; ++i){
        LinkLeft(NewNode(i), node_);
        node_ = node_->left;
    }
    TreeDump(node);*/

    char buffer[] = "(67(52(4)(8))(43(5(1)(2))(45)))";
    int ind = 0;
    TreeNode_t* node = ParseTree (buffer, &ind);
    Preorder (node);
    TreeDump (node);
    return 0;
}

TreeNode_t* NewNode (Elem_t data)
{
    TreeNode_t* node = (TreeNode_t*)calloc(1, sizeof (TreeNode_t));
    if (node == NULL)
    {
        printf ("Memory allocation error\n");
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

    node->data = -1;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    return true;
}

bool LinkLeft (TreeNode_t* child, TreeNode_t* parent)
{
    assert (child);
    assert (parent);

    if (parent->left != NULL)
    {
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

    if (parent->right != NULL)
    {
        printf ("Already has a right child\n");
        return false;
    }
    parent->right = child;
    child->parent = parent;
    return true;
}

void Preorder (TreeNode_t* root)
{
    printf("(%d", root->data);
    if (root->left) Preorder(root->left);
    if (root->right) Preorder(root->right);
    printf(")");
}

TreeNode_t* ParseTree (char* buffer, int* index)
{
    if (buffer[*index] != '(')
    {
        printf ("Error in read\n");
        exit (-1);
    }
    (*index)++;

    int num = 0, num_length = 0;
    sscanf (&buffer[*index], "%d%n", &num, &num_length);
    TreeNode_t* node = NewNode (num);
    (*index) += num_length;
    if (buffer[*index] == ')')
    {
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

void TreeDump (TreeNode_t* root)
{
    FILE* tree_dump = fopen ("tree_dump.txt", "w");
    fprintf (tree_dump, "digraph G{\nrankdir = TB;\n");
    fprintf (tree_dump, "node[fontsize = 14];\n");
    int ind = 0;
    NodeDump (root, &ind, tree_dump);
    fprintf (tree_dump, "}");
    fclose (tree_dump);
    system ("dot -Tpng tree_dump.txt -o tree.png");

}

int NodeDump (TreeNode_t* root, int* index, FILE* tree_dump)
{
    int node_id = *index;
    char* color_data = GenerateColor (root);
    char* color_parent = GenerateColor (root->parent);
    char* color_left_child = GenerateColor (root->left);
    char* color_right_child = GenerateColor (root->right);
    fprintf (tree_dump, "%d[shape = none, label =<"
                        "<table border = \"0\" cellborder = \"1\" cellspacing = \"0\">"
                        "<tr>"
                        "<td colspan=\"2\" bgcolor = \"%s\"> data = %d</td>"
                        "</tr>\n",
                        node_id, color_data, root->data);
    if (root->parent != NULL)
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
        color_left_child, root->left, color_right_child, root->right);
    fprintf(tree_dump, "</table>>];\n");
    (*index)++;
    if (root->left)
    {
        int left_node_id = NodeDump(root->left, index, tree_dump);
        fprintf (tree_dump, "%d->%d;\n", node_id, left_node_id);
    }
    if (root->right)
    {
        int right_node_id = NodeDump(root->right, index, tree_dump);
        fprintf (tree_dump, "%d->%d;\n", node_id, right_node_id);
    }
    return node_id;
}

char* GenerateColor (TreeNode_t* node)
{
    if (node == NULL)
        return strdup ("0 0 0");

    int ptr = (long long)node;
    srand(ptr);
    double h = (double)(rand () % 360) / 360;
    double s = 0.5;
    double v = 1;
    char *color = (char*)calloc(64, sizeof(char));
    sprintf (color, "%.3g %.3g %.3g", h, s, v);
    return color;
}
