#include "display/gui_global.h"
#include "display/MenuTree.h"
#include "measure/MeasureMan.h"
#include "sysMan/ViewSystem.h"
TreeMenu::TreeMenu(void) : m_showTooltip(true)
{
    m_treeView = 0;
}

gboolean HandleTreeViewMove(GtkWidget* widget,GdkEvent* event,gpointer data)
{
	//cout<<"just for test,don't mind'"<<endl;
}

GtkWidget* TreeMenu::Create(GUIMenu *pItem)
{
    /* creating a model */
    GtkTreeModel *model;
    model = CreateModel(pItem);

    /* creating the view */
    m_treeView = gtk_tree_view_new_with_model(model);
    AddColumns(GTK_TREE_VIEW(m_treeView));

    /* set view property */
    gtk_tree_view_set_enable_search(GTK_TREE_VIEW(m_treeView), FALSE);
    gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW(m_treeView), TRUE);
    // gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(m_treeView), TRUE);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(m_treeView), FALSE);
    //gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(m_treeView), TRUE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(m_treeView), TRUE);

    /* selection handling */
    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeView));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);
    g_signal_connect(select, "changed", G_CALLBACK(HandleTreeSelectionChanged), this);
     gtk_widget_add_events(GTK_WIDGET(m_treeView), (gtk_widget_get_events(GTK_WIDGET(m_treeView)) | GDK_BUTTON_RELEASE_MASK));
   g_signal_connect_after(m_treeView, "button_release_event", G_CALLBACK(HandleExamTypeBtnClicked), this);

    g_signal_connect(m_treeView, "row-expanded", G_CALLBACK(HandleTreeViewExpanded), this);
    g_signal_connect(m_treeView, "row-collapsed", G_CALLBACK(HandleTreeViewCollapse), this);
    //g_signal_connect(m_treeView, "row-active", G_CALLBACK(HandleTreeRowActivated), this);
  g_signal_connect(m_treeView, "test-expand-row", G_CALLBACK(HandleTreeViewExpandBefore), this);

    gtk_tree_view_collapse_all(GTK_TREE_VIEW(m_treeView));
//    gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeView));

//    g_object_unref(model);

    g_signal_connect(m_treeView, "motion-notify-event", G_CALLBACK(HandleTreeViewMove), this);
    return m_treeView;
}

GtkWidget* TreeMenu::Create(GUIMenu *pItem, GUIMenu *pUserDefine)
{
    /* creating a model */
    GtkTreeModel *model;
    model = CreateModel(pItem, pUserDefine);

    /* creating the view */
    m_treeView = gtk_tree_view_new_with_model(model);
    AddColumns(GTK_TREE_VIEW(m_treeView));

    /* set view property */
    gtk_tree_view_set_enable_search(GTK_TREE_VIEW(m_treeView), FALSE);
    gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW(m_treeView), TRUE);
    // gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(m_treeView), TRUE);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(m_treeView), FALSE);
    //gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(m_treeView), TRUE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(m_treeView), TRUE);

    /* selection handling */
    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeView));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

    g_signal_connect(select, "changed", G_CALLBACK(HandleTreeSelectionChanged), this);

    gtk_widget_add_events(GTK_WIDGET(m_treeView), (gtk_widget_get_events(GTK_WIDGET(m_treeView)) | GDK_BUTTON_RELEASE_MASK));

   g_signal_connect_after(m_treeView, "button_release_event", G_CALLBACK(HandleExamTypeBtnClicked), this);

    g_signal_connect(m_treeView, "row-expanded", G_CALLBACK(HandleTreeViewExpanded), this);
    g_signal_connect(m_treeView, "row-collapsed", G_CALLBACK(HandleTreeViewCollapse), this);
    //g_signal_connect(m_treeView, "row-active", G_CALLBACK(HandleTreeRowActivated), this);
  g_signal_connect(m_treeView, "test-expand-row", G_CALLBACK(HandleTreeViewExpandBefore), this);

    gtk_tree_view_collapse_all(GTK_TREE_VIEW(m_treeView));
//    gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeView));

//    g_object_unref(model);
    return m_treeView;
}

void TreeMenu::ChangeModel(GtkWidget *treeview, GUIMenu *pItem)
{
    GtkTreeModel *model;
    model = CreateModel(pItem);
    PRINTF("model address:%p\n", model);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), NULL);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), model);
    g_object_unref(model);
    gtk_widget_queue_draw(treeview);
    gtk_widget_show(treeview);
}

void TreeMenu::ChangeModel(GtkWidget *treeview, GUIMenu *pItem, GUIMenu *pUserDefine)
//void TreeMenu::ChangeModel(GtkWidget *treeview,  GUIMenu *pUserDefine)
{
    GtkTreeModel *model;
    model = CreateModel(pItem, pUserDefine);

    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), NULL);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), model);
    g_object_unref(model);
    gtk_widget_queue_draw(treeview);
    gtk_widget_show(treeview);
}

void TreeMenu::tree_auto_scroll(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data)
{
	const char *str="";
	static char lastStr[30] = "";
	bool d_press = (bool)user_data;
    str = gtk_tree_path_to_string(path);
    if (d_press)
    {
        gtk_tree_view_collapse_row(tree_view,
                gtk_tree_path_new_from_string(str));
        return;
    }
    if((!strstr(str, ":")) && (strcmp(lastStr, str)))
    {
        PRINTF("pathaaaaa:%s %s\n", str, lastStr);
        if(strcmp(lastStr, ""))
            gtk_tree_view_collapse_row(tree_view,
                   gtk_tree_path_new_from_string(lastStr));
        strcpy(lastStr, str);
    }

    gtk_tree_view_expand_row(tree_view, path, FALSE);
	gtk_tree_view_set_cursor(GTK_TREE_VIEW(tree_view), path, NULL, FALSE);
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(tree_view), path, NULL, TRUE, 0.5, 0.5);
 }

gboolean TreeMenu::BtnExamTypeClicked(GtkWidget *widget, GdkEventButton *event)
{
    if (event->button == 3)
        return FALSE;
    else if (event->button == 2)
        return FALSE;
    else if (event->button == 1)
    {
        if (event->type == GDK_BUTTON_RELEASE)
        {
            GtkTreeIter iter;
            GtkTreeModel *model;

            GtkTreeSelection *selection;
            selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));

            if (gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE)
            {
                GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
                //int tree_depth = gtk_tree_path_get_depth(path);
                gtk_tree_path_free (path);

                if(gtk_tree_model_iter_has_child(model, &iter))
                {
                    GtkTreePath *path=gtk_tree_model_get_path(model, &iter);
                   tree_auto_scroll(GTK_TREE_VIEW(widget), &iter, path, (gpointer)0);
                    gtk_tree_path_free(path);
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

void TreeMenu::TreeViewExpanded(GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path)
{
}

void TreeMenu::TreeViewCollapse(GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path)
{
}

//treeview展开之前做的事情，展开之前先收起所有的菜单
bool TreeMenu::TreeViewTestRowExpandBefore(GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path)
{
    gtk_tree_view_collapse_all(treeview);
    return FALSE; //必须要有返回值,才能触发事件
}

GtkWidget* TreeMenu::GetTreeView(void)
{
    return m_treeView;
}

void TreeMenu::SelectIter(GtkWidget *treeView, int parent, int child, bool select)
{
    // GtkTreeSelection *selection;
    // selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeView));

    // if (select)
    //  gtk_tree_selection_unselect_all(selection);
   char tmp[10];
    if (parent == -1)
        sprintf(tmp, "%d", child);
    else
        sprintf(tmp, "%d:%d", parent, child);
    GtkTreePath *path = gtk_tree_path_new_from_string(tmp);
    if (path != NULL)
    {
        gtk_tree_view_expand_to_path(GTK_TREE_VIEW(treeView), path);
        if (select) {
            // gtk_tree_selection_select_path(selection, path);
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(treeView), path, NULL, FALSE);
        }
        gtk_tree_path_free(path);
    }

}

void TreeMenu::TreeRowActivated(GtkTreeView *treeView, GtkTreePath *path, GtkTreeViewColumn *column)
{
    GtkTreeIter iter;
    PtrFunc func;
    int type, etype;

    GtkTreeModel *model;
    model = gtk_tree_view_get_model(treeView);

    gtk_tree_model_get_iter(model, &iter, path);

    //expand or collapse the parent node
    if (gtk_tree_model_iter_has_child(model, &iter)) {
        if (gtk_tree_view_row_expanded(treeView, path))
            gtk_tree_view_collapse_row(treeView, path);
        else
            gtk_tree_view_expand_to_path(treeView, path);
    }

    gtk_tree_model_get(model, &iter,
                       PFUNC_COLUMN, &func,
                       RENDERER_TYPE_COLUMN, &type,
                       ETYPE_COLUMN, &etype,
                       -1);
    switch (type) {
    case TEXT_ONLY:
        if (func.ptrNone)
            (func.ptrNone)(etype);
        break;
    case TOGGLE:
        //	    if (func.pf_int)
        //		(func.pf_int)();
        break;
    }
}

void TreeMenu::TreeSelectionChanged(GtkTreeSelection *selection)
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    PtrFunc func;
    int type, etype;

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE)
        return ;

    // GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    // GtkTreeView* treeview = gtk_tree_selection_get_tree_view (selection);

    // // expand or collapse the parent node
    // if (gtk_tree_model_iter_has_child(model, &iter)) {
    //     if (gtk_tree_view_row_expanded(GTK_TREE_VIEW(treeview), path)) {
    //         gtk_tree_view_collapse_row(GTK_TREE_VIEW(treeview), path);
    //     } else {
    //         gtk_tree_view_expand_to_path(GTK_TREE_VIEW(treeview), path);
    //     }
    // }
    // gtk_tree_path_free (path);

    gtk_tree_model_get(model, &iter,
                       PFUNC_COLUMN, &func,
                       RENDERER_TYPE_COLUMN, &type,
                       ETYPE_COLUMN, &etype,
                       -1);
    switch (type) {
    case TEXT_ONLY:
        if (func.ptrNone)
            (func.ptrNone)(etype);
        break;
    case TOGGLE:
        //	    if (func.pf_int)
        //		(func.pf_int)();
        break;
    }
}

// GtkTreeModel* TreeMenu::CreateModel(GUIMenu *pItem)
// {
//     GtkTreeIter iter;
//     GtkTreeStore *store = gtk_tree_store_new(N_COLUMNS, /* Total number of columns */
//                                              G_TYPE_STRING, /* Menu item name */
//                                              G_TYPE_INT, /* enum type */
//                                              G_TYPE_INT, /* Renderer type */
//                                              G_TYPE_INT, // toggle value
//                                              G_TYPE_POINTER); /* Union */
//     while (pItem->name) {
//         GUIMenu *pchild = pItem->child;
//         gtk_tree_store_append(store, &iter, NULL);
//         gtk_tree_store_set(store, &iter,
//                            NAME_COLUMN, _(pItem->name),
//                            ETYPE_COLUMN, pItem->etype,
//                            RENDERER_TYPE_COLUMN, pItem->type,
//                            VALUE_COLUMN, pItem->value,
//                            PFUNC_COLUMN, pItem->pf,
//                            -1);
//         pItem++;
//         if (!pchild)
//             continue;
//         while (pchild->name) {
//             GtkTreeIter child_iter;
//             gtk_tree_store_append(store, &child_iter, &iter);
//             gtk_tree_store_set(store, &child_iter,
//                                NAME_COLUMN, _(pchild->name),
//                                ETYPE_COLUMN, pchild->etype,
//                                RENDERER_TYPE_COLUMN, pchild->type,
//                                VALUE_COLUMN, pchild->value,
//                                PFUNC_COLUMN, pchild->pf,
//                                -1);
//             pchild++;
//         }
//     }
//     return GTK_TREE_MODEL(store);
// }

GtkTreeModel* TreeMenu::CreateModel(GUIMenu *pItem, GUIMenu *pUserDefine)
{
    GtkTreeIter iter;
    GtkTreeStore *store = gtk_tree_store_new(N_COLUMNS, /* Total number of columns */
            G_TYPE_STRING, /* Menu item name */
            G_TYPE_INT, /* enum type */
            G_TYPE_INT, /* Renderer type */
            G_TYPE_INT, // toggle value
            G_TYPE_POINTER); /* Union */

    while (pItem->name || pItem->etype >= 0) {
        GUIMenu *pchild = pItem->child;
        const char *name;
        if(pItem->etype >= BASIC_MEA_END)
            name = MeasureMan::GetInstance()->GetMeasureTitle(pItem->etype);
        else
            name = pItem->name;

        if(name)
        {
            gtk_tree_store_append(store, &iter, NULL);
            gtk_tree_store_set(store, &iter,
                    //	NAME_COLUMN, _(pItem->name),
                    NAME_COLUMN, _(name),
                    ETYPE_COLUMN, pItem->etype,

                    RENDERER_TYPE_COLUMN, pItem->type,
                    VALUE_COLUMN, pItem->value,
                    PFUNC_COLUMN, pItem->pf,
                    -1);
        }
        pItem++;
        if (!pchild)
            continue;

        while (pchild->etype >= 0) {
            const char *child_name;
            if(pchild->etype >= BASIC_MEA_END)
                child_name = MeasureMan::GetInstance()->GetMeasureTitle(pchild->etype);
            else
                child_name = pchild->name;
            if(child_name)
            {
                GtkTreeIter child_iter;
                gtk_tree_store_append(store, &child_iter, &iter);
                gtk_tree_store_set(store, &child_iter,
                        NAME_COLUMN, _(child_name),
                        ETYPE_COLUMN, pchild->etype,
                        RENDERER_TYPE_COLUMN, pchild->type,
                        VALUE_COLUMN, pchild->value,
                        PFUNC_COLUMN, pchild->pf,
                        -1);
            }
            pchild++;
        }
    }
    while (pUserDefine->name || pUserDefine->etype >= 0) {
        GUIMenu *pUserDefinechild = pUserDefine->child;
        const char *nametitle;
        if(pUserDefine->etype >= BASIC_MEA_END)
            nametitle = MeasureMan::GetInstance()->GetMeasureTitle(pUserDefine->etype); // need to modify lhm
        else
            nametitle = pUserDefine->name;

        if(nametitle)
        {
            gtk_tree_store_append(store, &iter, NULL);
            gtk_tree_store_set(store, &iter,
                    //	NAME_COLUMN, _(pItem->name),
                    NAME_COLUMN, _(nametitle),
                    ETYPE_COLUMN, pUserDefine->etype,
                    RENDERER_TYPE_COLUMN, pUserDefine->type,
                    VALUE_COLUMN, pUserDefine->value,
                    PFUNC_COLUMN, pUserDefine->pf,
                    -1);
        }
        pUserDefine++;
        if (!pUserDefinechild)
            continue;

        while (pUserDefinechild->etype >= 0) {
            const char *userDefine_child_name;
            if(pUserDefinechild->etype >= BASIC_MEA_END)
                userDefine_child_name = MeasureMan::GetInstance()->GetMeasureTitle(pUserDefinechild->etype); // need to modify lhm
            else
                userDefine_child_name = pUserDefinechild->name;
            if(userDefine_child_name)
            {
                GtkTreeIter userdefine_child_iter;
                gtk_tree_store_append(store, &userdefine_child_iter, &iter);
                gtk_tree_store_set(store, &userdefine_child_iter,
                        NAME_COLUMN, _(userDefine_child_name),
                        ETYPE_COLUMN, pUserDefinechild->etype,
                        RENDERER_TYPE_COLUMN, pUserDefinechild->type,
                        VALUE_COLUMN, pUserDefinechild->value,
                        PFUNC_COLUMN, pUserDefinechild->pf,
                        -1);
            }
            pUserDefinechild++;
        }
    }
    return GTK_TREE_MODEL(store);
}

void TreeMenu::Update(GtkWidget *treeView, GUIMenu *pItem, GUIMenu *pUserDefine)
{
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeView));
    GtkTreeIter iter;
    gtk_tree_model_get_iter_first(model, &iter);

    while (pItem->name) {
        GUIMenu *pchild = pItem->child;
        gtk_tree_store_set(GTK_TREE_STORE(model), &iter, NAME_COLUMN, (pItem->name), -1);
        pItem++;

        if (!pchild) {
            gtk_tree_model_iter_next(model, &iter);
            continue;
        }

        GtkTreeIter child_iter;
        gtk_tree_model_iter_children(model, &child_iter, &iter);
        while (pchild->name) {
            gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter, NAME_COLUMN, (pchild->name), -1);
            pchild++;
            gtk_tree_model_iter_next(model, &child_iter);
        }
        gtk_tree_model_iter_next(model, &iter);
    }

    while (pUserDefine->name) {
        GUIMenu *puserdefinechild = pUserDefine->child;
        gtk_tree_store_set(GTK_TREE_STORE(model), &iter, NAME_COLUMN, (pUserDefine->name), -1);
        pUserDefine++;

        if (!puserdefinechild) {
            gtk_tree_model_iter_next(model, &iter);
            continue;
        }

        GtkTreeIter user_define_child_iter;
        gtk_tree_model_iter_children(model, &user_define_child_iter, &iter);
        while (puserdefinechild->name) {
            gtk_tree_store_set(GTK_TREE_STORE(model), &user_define_child_iter, NAME_COLUMN, (puserdefinechild->name), -1);
            puserdefinechild++;
            gtk_tree_model_iter_next(model, &user_define_child_iter);
        }
        gtk_tree_model_iter_next(model, &iter);
    }

}

GtkTreeModel* TreeMenu::CreateModel(GUIMenu *pItem)
{
    GtkTreeIter iter;
    GtkTreeStore *store = gtk_tree_store_new(N_COLUMNS, /* Total number of columns */
            G_TYPE_STRING, /* Menu item name */
            G_TYPE_INT, /* enum type */
            G_TYPE_INT, /* Renderer type */
            G_TYPE_INT, // toggle value
            G_TYPE_POINTER); /* Union */

#ifdef EMP_3410
    if(pItem->name != NULL)
        if(!CManRegister::GetInstance()->IsAuthorize("IMT"))
            if(strcmp(pItem->name, "IMT") == 0)
            {
                pItem->name = NULL;
                pItem->etype = -1;
            }
#endif

    while (pItem->name || pItem->etype >= 0) {
        GUIMenu *pchild = pItem->child;
        const char *name;
        if(pItem->etype >= BASIC_MEA_END)
            name = MeasureMan::GetInstance()->GetMeasureTitle(pItem->etype);
        else
            name = pItem->name;

        if(name)
        {
            gtk_tree_store_append(store, &iter, NULL);
            gtk_tree_store_set(store, &iter,
                    //	NAME_COLUMN, _(pItem->name),
                    NAME_COLUMN, _(name),
                    ETYPE_COLUMN, pItem->etype,

                    RENDERER_TYPE_COLUMN, pItem->type,
                    VALUE_COLUMN, pItem->value,
                    PFUNC_COLUMN, pItem->pf,
                    -1);
        }
        pItem++;
#ifdef EMP_3410
        if(pItem->name != NULL)
            if(!CManRegister::GetInstance()->IsAuthorize("IMT"))
                if(strcmp(pItem->name, "IMT") == 0)
                {
                    pItem->name = NULL;
                    pItem->etype = -1;
                }
#endif

        if (!pchild)
            continue;

        while (pchild->etype >= 0) {
            const char *child_name;
            if(pchild->etype >= BASIC_MEA_END)
                child_name = MeasureMan::GetInstance()->GetMeasureTitle(pchild->etype);
            else
                child_name = pchild->name;

            if(child_name)
            {
                GtkTreeIter child_iter;
                gtk_tree_store_append(store, &child_iter, &iter);
                gtk_tree_store_set(store, &child_iter,
                        NAME_COLUMN, _(child_name),
                        ETYPE_COLUMN, pchild->etype,
                        RENDERER_TYPE_COLUMN, pchild->type,
                        VALUE_COLUMN, pchild->value,
                        PFUNC_COLUMN, pchild->pf,
                        -1);
            }
            pchild++;
        }
    }
    return GTK_TREE_MODEL(store);
}

void TreeMenu::Update(GtkWidget *treeView, GUIMenu *pItem)
{
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeView));
    GtkTreeIter iter;
    gtk_tree_model_get_iter_first(model, &iter);

    while (pItem->name) {
        GUIMenu *pchild = pItem->child;
        gtk_tree_store_set(GTK_TREE_STORE(model), &iter, NAME_COLUMN, _(pItem->name), -1);
        pItem++;

        if (!pchild) {
            gtk_tree_model_iter_next(model, &iter);
            continue;
        }

        GtkTreeIter child_iter;
        gtk_tree_model_iter_children(model, &child_iter, &iter);
        while (pchild->name) {
            gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter, NAME_COLUMN, _(pchild->name), -1);
            pchild++;
            gtk_tree_model_iter_next(model, &child_iter);
        }
        gtk_tree_model_iter_next(model, &iter);
    }
}

void TreeMenu::AddColumns(GtkTreeView *treeView)
{
    /* column for menu name */
    GtkCellRenderer *renderer_text;
    GtkCellRenderer *renderer_toggle;
    GtkTreeViewColumn *column_text;
    GtkTreeViewColumn *column_toggle;

    renderer_text = gtk_cell_renderer_text_new(); // GtkCellRendererText
    column_text = gtk_tree_view_column_new_with_attributes("ITEM_NAME", // column name
							   renderer_text,    // the cell renderer type
							   "text", NAME_COLUMN, // renderer attribute
							   NULL);
    g_object_set(G_OBJECT(column_text),
		 "sizing", GTK_TREE_VIEW_COLUMN_FIXED,
		 "fixed_width", 180,
		 NULL);

    renderer_toggle = gtk_cell_renderer_toggle_new();
    column_toggle = gtk_tree_view_column_new_with_attributes("TOGGLE",
							     renderer_toggle,
							     "visible", RENDERER_TYPE_COLUMN,
							     "active", VALUE_COLUMN,
							     NULL);
    g_object_set(G_OBJECT(column_toggle),
		 "sizing", GTK_TREE_VIEW_COLUMN_FIXED,
		 "fixed_width", 50,
		 NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column_text);
//    gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column_toggle);
}

void TreeMenu::AddColumnsNew(GtkTreeView *treeView)
{
    /* column for menu name */
    GtkCellRenderer *renderer_text;
    GtkCellRenderer *renderer_toggle;
    GtkTreeViewColumn *column_text;
    GtkTreeViewColumn *column_toggle;

    renderer_text = gtk_cell_renderer_text_new(); // GtkCellRendererText
	g_object_set(G_OBJECT(renderer_text), "ellipsize", PANGO_ELLIPSIZE_END, NULL);
    column_text = gtk_tree_view_column_new_with_attributes("ITEM_NAME", // column name
							   renderer_text,    // the cell renderer type
							   "text", NAME_COLUMN, // renderer attribute
							   NULL);
    g_object_set(G_OBJECT(column_text),
		 "sizing", GTK_TREE_VIEW_COLUMN_FIXED,
		 "fixed_width", NAME_COLUMN_WIDTH,
		 NULL);

    renderer_toggle = gtk_cell_renderer_toggle_new();
    column_toggle = gtk_tree_view_column_new_with_attributes("TOGGLE",
							     renderer_toggle,
							     "visible", RENDERER_TYPE_COLUMN,
							     "active", VALUE_COLUMN,
							     NULL);
    g_object_set(G_OBJECT(column_toggle),
		 "sizing", GTK_TREE_VIEW_COLUMN_FIXED,
		 "fixed_width", RENDERER_TYPE_COLUMN_WIDTH,
		 NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column_text);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column_toggle);
}

void TreeMenu::SelectFirst(GtkWidget *tree_view)
{
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    GtkTreeModel *model;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view));

    if(gtk_tree_model_get_iter_first(model, &iter))
        SelectItemNew(tree_view, model, selection, &iter);
}

void TreeMenu::SelectNext(GtkWidget *tree_view, EMeasureSequenceType sequence)
{
	GtkTreeIter iter, iterParent;
	GtkTreeModel *model;

	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));

	if (gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE)
    {
        if (gtk_tree_model_iter_parent(model, &iterParent, &iter) == TRUE)
        {

            if (gtk_tree_model_iter_next(model, &iter) == TRUE)
                SelectItemNew(tree_view, model, selection, &iter);
            else
            {
                if (sequence == MEA_SEQ_NEXT)
                {
                    gtk_tree_view_collapse_all(GTK_TREE_VIEW(tree_view));
                    if (gtk_tree_model_iter_next(model, &iterParent) == TRUE)
                        SelectItemNew(tree_view, model, selection, &iterParent);
                }
            }

        }
		else
        {
            if (sequence == MEA_SEQ_NEXT)
            {
                if (gtk_tree_model_iter_next(model, &iter) == TRUE)
                    SelectItemNew(tree_view, model, selection, &iter);
            }
        }
    }
}

void TreeMenu::SelectItemNew(GtkWidget *tree_view, GtkTreeModel *model, GtkTreeSelection *selection, GtkTreeIter *iter)
{
    gtk_tree_selection_select_iter(selection, iter);
    PtrFunc func;
    int type, etype;
    gtk_tree_model_get(model, iter,
            PFUNC_COLUMN, &func,
            RENDERER_TYPE_COLUMN, &type,
            ETYPE_COLUMN, &etype,
            -1);
    switch (type)
    {
        case TEXT_ONLY:
        case TOGGLE:
            if (func.ptrNone)
                (func.ptrNone)(etype);
            break;
    }

    gtk_tree_selection_get_selected(selection, &model, iter);
    gtk_tree_selection_get_selected(selection, &model, &m_iterBak);
    GtkTreePath *path = gtk_tree_model_get_path(model, iter);
    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(tree_view), path, NULL, TRUE, 1, 0.5);
    //gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(tree_view), path, NULL, FALSE, 0.5, 0.5);
    gtk_tree_path_free(path);

    g_timeout_add(200, TimeoutMoveCursor, (gpointer)this);
}

gboolean TreeMenu::TimeoutMoveCursor(gpointer data)
{
	if ((TreeMenu *)data != NULL)
	{
		GtkWidget *tree_view = ((TreeMenu *)data)->m_treeView;
		GtkTreeIter iter;
		GtkTreeModel *model;

		GtkTreeSelection *selection;
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));

        if (&(((TreeMenu *)data)->m_iterBak) != NULL)
            gtk_tree_selection_select_iter(selection, &(((TreeMenu *)data)->m_iterBak));

		if (gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE)
		{
			GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
			GdkRectangle rect;
			gtk_tree_view_get_cell_area(GTK_TREE_VIEW(tree_view), path, gtk_tree_view_get_column(GTK_TREE_VIEW(tree_view), 0), &rect);
			gtk_tree_path_free(path);

			int x, y;
			gtk_widget_get_pointer(tree_view, &x, &y);
			int offsetY = rect.y + 5 - y;
			fakeXMotionEvent(0, -offsetY);
		}
	}
	return FALSE;
}

GtkWidget* TreeMenu::Create(vector<GUIMenuNew> &vecItems)
{
    /* creating a model */
    GtkTreeModel *model;
    model = CreateModel(vecItems);

    /* creating the view */
    m_treeView = gtk_tree_view_new_with_model(model);
    AddColumnsNew(GTK_TREE_VIEW(m_treeView));

    /* set view property */
    gtk_tree_view_set_enable_search(GTK_TREE_VIEW(m_treeView), FALSE);
    gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW(m_treeView), TRUE);
    gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(m_treeView), TRUE);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(m_treeView), FALSE);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(m_treeView), TRUE);
	//gtk_tree_view_set_show_expanders(GTK_TREE_VIEW(m_treeView), FALSE);
	gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(m_treeView), GTK_TREE_VIEW_GRID_LINES_NONE);
	gtk_widget_set(m_treeView, "has-tooltip", TRUE, NULL);

	/* selection handling */
	GtkTreeSelection *select;
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeView));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
	gtk_widget_add_events(GTK_WIDGET(m_treeView), (gtk_widget_get_events(GTK_WIDGET(m_treeView)) | GDK_BUTTON_PRESS_MASK));
	g_signal_connect(m_treeView, "button_press_event", G_CALLBACK(HandleTreeViewClicked), this);
	//g_signal_connect (G_OBJECT(m_treeView), "motion-notify-event", G_CALLBACK(HandleTreeViewMouseMove), this);
	g_signal_connect (G_OBJECT(m_treeView), "query-tooltip", G_CALLBACK(HandleTreeViewTooltip), this);

	gtk_tree_view_collapse_all(GTK_TREE_VIEW(m_treeView));

	g_object_unref(model);
	return m_treeView;
}

GtkTreeModel* TreeMenu::CreateModel(vector<GUIMenuNew> &vecItems)
{
    GtkTreeIter iter;
    GtkTreeStore *store = gtk_tree_store_new(N_COLUMNS, /* Total number of columns */
            G_TYPE_STRING, /* Menu item name */
            G_TYPE_INT, /* enum type */
            G_TYPE_INT, /* Renderer type */
            G_TYPE_INT, // toggle value
            G_TYPE_POINTER); /* Union */

	if (vecItems.size() <= 0)
		return GTK_TREE_MODEL(store);

	vector<GUIMenuNew>::iterator pItem = vecItems.begin();
    while (pItem != vecItems.end())
	{
      vector<GUIMenuNew> vecChild = pItem->child;
        const char *name;

        if (pItem->etype >= BASIC_MEA_START && pItem->etype < USER_START)
		{
            name = MeasureMan::GetInstance()->GetMeasureTitle(pItem->etype);
		}
        else
            name = pItem->name;

        if (name != NULL)
        {
            gtk_tree_store_append(store, &iter, NULL);
            gtk_tree_store_set(store, &iter,
                    NAME_COLUMN, _(name),
                    ETYPE_COLUMN, pItem->etype,

                    RENDERER_TYPE_COLUMN, pItem->type,
                    VALUE_COLUMN, pItem->value,
                    PFUNC_COLUMN, pItem->pf,
                    -1);
        }
        pItem++;
        if (vecChild.size() <= 0)
            continue;

        vector<GUIMenuNew>::iterator pchild = vecChild.begin();
        while (pchild != vecChild.end())
		{
            const char *child_name;
            if(pchild->etype >= BASIC_MEA_START && pchild->etype < USER_START)   //pItem->etype < USER_START)
            {
                child_name = MeasureMan::GetInstance()->GetMeasureTitle(pchild->etype);
            }
            else
            {
                child_name = pchild->name;
            }

            if(child_name != NULL)
            {
                GtkTreeIter child_iter;
                gtk_tree_store_append(store, &child_iter, &iter);
                gtk_tree_store_set(store, &child_iter,
                        NAME_COLUMN, _(child_name),
                        ETYPE_COLUMN, pchild->etype,
                        RENDERER_TYPE_COLUMN, pchild->type,
                        VALUE_COLUMN, pchild->value,
                        PFUNC_COLUMN, pchild->pf,
                        -1);
            }
            pchild++;
        }
    }

    return GTK_TREE_MODEL(store);
}

void TreeMenu::ChangeModel(GtkWidget *treeview, vector<GUIMenuNew> &vecItems)
{
    GtkTreeModel *model;
    model = CreateModel(vecItems);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), NULL);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), model);
    g_object_unref(model);
    gtk_widget_queue_draw(treeview);
    gtk_widget_show(treeview);
}

void TreeMenu::ExpandMenu(GtkWidget *tree_view)
{
	GtkTreeIter iter, iterChild;
	GtkTreeModel *model;

	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));

	if (gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE)
	{
		if(gtk_tree_model_iter_has_child(model, &iter))
		{
			GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
			gtk_tree_view_expand_to_path(GTK_TREE_VIEW(tree_view), path);
			gtk_tree_path_free(path);

			gtk_tree_model_iter_children(model, &iterChild, &iter);
			gtk_tree_selection_select_iter(selection, &iterChild);
			PtrFunc func;
			int type, etype;
			gtk_tree_model_get(model, &iterChild,
					PFUNC_COLUMN, &func,
					RENDERER_TYPE_COLUMN, &type,
					ETYPE_COLUMN, &etype,
					-1);
			switch (type)
			{
				case TEXT_ONLY:
				case TOGGLE:
					if (func.ptrNone)
						(func.ptrNone)(etype);
					break;
			}

			path = gtk_tree_model_get_path(model, &iterChild);
		//	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(tree_view), path, NULL, TRUE, 0.5, 0.5);
			gtk_tree_path_free(path);
		}
	}
}

gboolean TreeMenu::TreeViewClicked(GtkWidget *widget, GdkEventButton *event)
{
    if (event->button == 3)
        return FALSE;
    else if (event->button == 2)
        return FALSE;
    else if (event->button == 1)
    {
        if (event->type == GDK_BUTTON_PRESS)
        {
            GtkTreeIter iter, iterParent;
            GtkTreeModel *model;

            GtkTreeSelection *selection;
            selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));

            if (gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE)
            {
				if (!gtk_tree_model_iter_parent(model, &iterParent, &iter))
				{
					gtk_tree_view_collapse_all(GTK_TREE_VIEW(m_treeView));
					gtk_tree_selection_select_iter(selection, &iter);
				}

				PtrFunc func;
				int type, etype;
				gtk_tree_model_get(model, &iter,
						PFUNC_COLUMN, &func,
						RENDERER_TYPE_COLUMN, &type,
						ETYPE_COLUMN, &etype,
						-1);
				switch (type)
				{
					case TEXT_ONLY:
					case TOGGLE:
						if (func.ptrNone)
							(func.ptrNone)(etype);
						break;
				}
				return TRUE;
            }
        }
    }
    return TRUE;
}

gboolean TreeMenu::TreeViewMouseMove(GtkWidget *widget, GdkEventMotion *event)
{
	gint bx, by;
	gtk_tree_view_convert_widget_to_bin_window_coords(GTK_TREE_VIEW(widget), event->x, event->y, &bx, &by);

	GtkTreePath *path = NULL;
	GtkTreeViewColumn *column = NULL;
	gint cell_x, cell_y;
	gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(widget), bx, by, &path, &column, &cell_x, &cell_y);
	gtk_tree_view_set_cursor(GTK_TREE_VIEW(widget), path, NULL, FALSE);
	gtk_tree_path_free(path);
	return FALSE;
}

gboolean TreeMenu::TreeViewTooltip(GtkWidget *widget, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip)
{
	if (!m_showTooltip)
	{
		m_showTooltip = true;
		return FALSE;
	}

	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));

	if (gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE)
	{
		gchar *text;
		gtk_tree_model_get(model, &iter, NAME_COLUMN, &text, -1);

		int width, height;
		PangoLayout *layout = gtk_widget_create_pango_layout(widget, text);
		GtkStyle *style = gtk_widget_get_style(widget);
		pango_layout_set_font_description(layout, style->font_desc);
		pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
		pango_layout_get_pixel_size(layout, &width, &height);
		//printf("========================width=%d height=%d\n", width, height); //height=17

		if (width > NAME_COLUMN_WIDTH - 25)
		{
			gtk_tooltip_set_text(tooltip, text);
			GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
			gtk_tree_view_set_tooltip_row(GTK_TREE_VIEW(widget), tooltip, path);
			gtk_tree_path_free(path);
			g_free(text);
			return TRUE;
		}
		g_free(text);
	}

	return FALSE;
}

int TreeMenu::SetCompleteMeasureFlag(GtkWidget *tree_view, int &parentEtype)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	int etype = BASIC_MEA_END;
	parentEtype = -1;

	if (gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE)
	{
		gtk_tree_store_set((GtkTreeStore *)model, &iter, RENDERER_TYPE_COLUMN, TOGGLE, -1);
		gtk_tree_model_get(model, &iter, ETYPE_COLUMN, &etype, -1);

		GtkTreeIter iterParent;
		if (gtk_tree_model_iter_parent(model, &iterParent, &iter) == TRUE)
		{
			if (IsSetParentCompleteMeasureFlag(model, &iterParent))
			{
				gtk_tree_store_set((GtkTreeStore *)model, &iterParent, RENDERER_TYPE_COLUMN, TOGGLE, -1);
                gtk_tree_model_get(model, &iterParent, ETYPE_COLUMN, &parentEtype, -1);
            }
        }

        if((etype >= OB_BPD)&&(etype <= OB_APAD))
        {
            GtkTreeIter tmp_iter;
            char *strtmp = NULL;
            gboolean has_node = gtk_tree_model_get_iter_first(model, &tmp_iter);
            while (has_node == TRUE)
            {
                gtk_tree_model_get(model, &tmp_iter, 0, &strtmp, -1);
                if (strcmp(strtmp, CalcSetting::GetInstance()->ItemMenuTransEnglish(etype-BASIC_MEA_END).c_str()) == 0)
                {
                    gtk_tree_store_set((GtkTreeStore *)model, &tmp_iter, RENDERER_TYPE_COLUMN, TOGGLE, -1);
                    has_node = gtk_tree_model_iter_next(model, &tmp_iter);
                    //break;
                }
                else if(strcmp(strtmp , _("EFW"))==0)
                {
                    GtkTreeIter child_iter;
                    gboolean has_child = gtk_tree_model_iter_children(model, &child_iter, &tmp_iter);
                    while (has_child == TRUE)
                    {
                        gtk_tree_model_get(model, &child_iter, 0, &strtmp, -1);
                        if (strcmp(strtmp, CalcSetting::GetInstance()->ItemMenuTransEnglish(etype-BASIC_MEA_END).c_str()) == 0)
                        {
                            gtk_tree_store_set((GtkTreeStore *)model, &child_iter, RENDERER_TYPE_COLUMN, TOGGLE, -1);
                            has_child = gtk_tree_model_iter_next(model, &child_iter);
                        }
                        else
                            has_child = gtk_tree_model_iter_next(model, &child_iter);
                    }
                    has_node = gtk_tree_model_iter_next(model, &tmp_iter);

                }
                else
                    has_node = gtk_tree_model_iter_next(model, &tmp_iter);
            }

        }
    }

	return etype;
}

bool TreeMenu::IsSetParentCompleteMeasureFlag(GtkTreeModel *tree_model, GtkTreeIter *iter)
{
	bool isSet = true;

    GtkTreeIter iterChild;
	RendererType type;
    int etype = BASIC_MEA_END;

	int number = gtk_tree_model_iter_n_children(tree_model, iter);
	for (int i = 0; i < number; i++)
	{
        if (gtk_tree_model_iter_nth_child(tree_model, &iterChild, iter, i))
        {
            gtk_tree_model_get(tree_model, &iterChild, RENDERER_TYPE_COLUMN, &type, -1);
            gtk_tree_model_get(tree_model, &iterChild, ETYPE_COLUMN, &etype, -1);
            if((etype >= OB_AFI_LUQ)&&(etype <= OB_AFI_RLQ))
            {
                if (type == TOGGLE)
                {
                    isSet = true;
                    break;
                }

            }
            else
            {
                if (type == TEXT_ONLY)
                {
                    isSet = false;
                    break;
                }
            }
        }
    }

	return isSet;
}

void TreeMenu::Update(GtkWidget *treeView, vector<GUIMenuNew> &vecItems)
{
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeView));
    GtkTreeIter iter;
    gtk_tree_model_get_iter_first(model, &iter);

	vector<GUIMenuNew>::iterator pItem = vecItems.begin();
    while (pItem != vecItems.end())
	{
        vector<GUIMenuNew> vecChild = pItem->child;

        const char *name;
		if (pItem->etype >= BASIC_MEA_START && pItem->etype < USER_START)
		{
            name = MeasureMan::GetInstance()->GetMeasureTitle(pItem->etype);
		}
        else
            name = pItem->name;

        if (name != NULL)
			gtk_tree_store_set(GTK_TREE_STORE(model), &iter, NAME_COLUMN, _(name), -1);

        pItem++;
        if (vecChild.size() <= 0)
		{
            gtk_tree_model_iter_next(model, &iter);
            continue;
		}

        GtkTreeIter child_iter;
        gtk_tree_model_iter_children(model, &child_iter, &iter);
		vector<GUIMenuNew>::iterator pchild = vecChild.begin();
        while (pchild != vecChild.end())
		{
            const char *child_name;
            if(pchild->etype >= BASIC_MEA_START && pItem->etype < USER_START)
                child_name = MeasureMan::GetInstance()->GetMeasureTitle(pchild->etype);
            else
                child_name = pchild->name;

            if(child_name != NULL)
				gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter, NAME_COLUMN, _(child_name), -1);
            pchild++;
            gtk_tree_model_iter_next(model, &child_iter);
        }

        gtk_tree_model_iter_next(model, &iter);
	}
}