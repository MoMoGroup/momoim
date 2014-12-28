#include <gtk/gtk.h>

// 使用GtkTreeModel作为下拉列表框的数据来源
static GtkWidget *create_combobox_with_model()
{
    GtkWidget *combobox = NULL;
    GtkListStore *store = NULL;
    GtkTreeIter iter;
    GtkCellRenderer *renderer = NULL;

    // 填充数据。在这里只需要一列字符串。
    store = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "home", -1);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "work", -1);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "public", -1);

    combobox = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
    // 设置GtkTreeModel中的每一项数据如何在列表框中显示
    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combobox), renderer, TRUE);
    gtk_cell_layout_set_attributes(
            GTK_CELL_LAYOUT(combobox), renderer, "text", 0, NULL);
    return combobox;
}

static void
add_item_clicked(GtkWidget *sender, gpointer data)
{
    GtkWidget *combobox;
    GtkTreeModel *model;
    GtkTreeIter iter;

    // 给列表框添加一个新条目，实际上就是给它背后的GtkTreeModel增加一个数据项。
    // 这是GTK+的模型-视图模式带来的巨大好处。类似也能用在GtkTreeView中，等等。
    combobox = (GtkWidget *)data;
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(combobox));
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, "new item", -1);
}

// 假设：能找到要选择的列表项。
static void
select_item_clicked(GtkWidget *sender, gpointer data)
{
    GtkWidget *combobox;
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *text = NULL;
    gboolean end_search = FALSE;

    combobox = (GtkWidget *)data;

    // 先从model中找到要选择的字符串，用iter记住位置
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(combobox));
    if (gtk_tree_model_get_iter_first(model, &iter))
    {
        do
        {
            gtk_tree_model_get(model, &iter, 0, &text, -1);
            end_search = g_strcmp0(text, "work") == 0;
            g_free(text);
        }
        while (!end_search && gtk_tree_model_iter_next(model, &iter));
    }

    // 用iter选择那一项。
    gtk_combo_box_set_active_iter(GTK_COMBO_BOX(combobox), &iter);
}

int main(int argc, char *argv[])
{
    GtkWidget *dialog, *box, *button, *combobox;

    gtk_init(&argc, &argv);

    dialog = gtk_dialog_new_with_buttons(
            "Test GtkComboBox",
            NULL,
            GTK_DIALOG_MODAL,
            GTK_STOCK_OK, GTK_RESPONSE_OK,
            NULL);

    // 作为演示，首先在对话框的内容区域（content_area）添加一个GtkComboBox。
    // 然后，在行为区域（action_area）添加两个额外的按钮，
    // 一个用于添加新条目，另一个用于选择某个条目。
    box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    combobox = create_combobox_with_model();
    gtk_box_pack_start(GTK_BOX(box), combobox, TRUE, TRUE, 3);

    button = gtk_button_new_with_label("Add item");
    box = gtk_dialog_get_action_area(GTK_DIALOG(dialog));
    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);
    g_signal_connect(
            G_OBJECT(button), "clicked",
            G_CALLBACK(add_item_clicked), combobox);

    button = gtk_button_new_with_label("Select work");
    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);
    g_signal_connect(
            G_OBJECT(button), "clicked",
            G_CALLBACK(select_item_clicked), combobox);

    gtk_widget_show_all(dialog);
    // 忽略对话框的返回值
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return 0;
}