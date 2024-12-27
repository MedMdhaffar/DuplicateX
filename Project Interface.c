#include <gtk/gtk.h>
#include "printfiles.c" //mdhaffar
#include "find_duplicate_main.c" //Iyed


static void delete_duplicate_files(HashEntry *hash_map, size_t hash_count) {
    for (size_t i = 0; i < hash_count; i++) {
        for (size_t j = 1; j < hash_map[i].file_count; j++) {
            if (remove(hash_map[i].file_paths[j]) == 0) {
                printf("Deleted: %s\n", hash_map[i].file_paths[j]);
            } else {
                fprintf(stderr, "Error deleting %s: %s\n", hash_map[i].file_paths[j], strerror(errno));
            }
        }
    }
}

static void on_showDuplicateFilesButton_clicked(GtkWidget *button, gpointer user_data) {
    GtkWidget **widgets = (GtkWidget **)user_data; //Array mt3 widgets
    GtkWidget *entry = widgets[0];             //el blasa eli n7otou feha el path     
    GtkWidget *label = widgets[1];             // el blasa eli feha el output   

    // Set up hash variables
    HashEntry hash_map[1000];
    size_t hash_count = 0; 

    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(entry)); //el buffer mt3 el text (text holder)
    const gchar *text = gtk_entry_buffer_get_text(buffer);  //el pointer mt3 el text
    if (text != NULL && text[0] != '\0') {
        gchar *result;
        result = print_duplicate_files(text, hash_map, &hash_count); //result text (el output mt3 el function duplicate files)
        printf("%s", result);
        gchar *output_text = g_strdup_printf("Your duplicate files in : %s are : \n %s", text, result);
        gtk_label_set_text(GTK_LABEL(label), output_text);  //set output in the label
        g_free(output_text);
    }
    //free memory for hashmap
    for (size_t i = 0; i < hash_count; i++) {
        for (size_t j = 0; j < hash_map[i].file_count; j++) {
            free(hash_map[i].file_paths[j]);
        }
        free(hash_map[i].file_paths);
    }
}

static void on_showFilesButton_clicked(GtkWidget *button, gpointer user_data) {
    GtkWidget **widgets = (GtkWidget **)user_data;
    GtkWidget *entry = widgets[0];              
    GtkWidget *label = widgets[1];

    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(entry));    
    const gchar *text = gtk_entry_buffer_get_text(buffer);
    gchar *result;
    result = print_existing_files(text);

    gchar *output_text = g_strdup_printf("Your files in : %s are : \n %s", text, result);
    gtk_label_set_text(GTK_LABEL(label), output_text);
    g_free(output_text);
}

static void on_yes_button_clicked(GtkWidget *button, gpointer user_data) {
    GtkWidget **widgets = (GtkWidget **)user_data;
    GtkWidget *entry = widgets[0];
    GtkWidget *label = widgets[1];
    GtkWidget *subwindow = widgets[2];      //Alert Window totl3 ki ta3ml delete !!

    HashEntry hash_map[1000];
    size_t hash_count = 0;

    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(entry));
    const gchar *text = gtk_entry_buffer_get_text(buffer);

    if (text != NULL && text[0] != '\0') {
        print_duplicate_files(text, hash_map, &hash_count);
        delete_duplicate_files(hash_map, hash_count);
        gchar *output_text = g_strdup_printf("Duplicate files in %s have been deleted.", text);
        gtk_label_set_text(GTK_LABEL(label), output_text);
        g_free(output_text);
    }
    //free allocated memroy for hasmap
    for (size_t i = 0; i < hash_count; i++) {
        for (size_t j = 0; j < hash_map[i].file_count; j++) {
            free(hash_map[i].file_paths[j]);
        }
        free(hash_map[i].file_paths);
    }
    gtk_window_destroy(GTK_WINDOW(subwindow));  //ta3ml destroy lel alert ki tenzel yes
}

static void on_no_button_clicked(GtkWidget *button, gpointer user_data) {
    GtkWidget **widgets = (GtkWidget **)user_data;
    GtkWidget *entry = widgets[0];
    GtkWidget *label = widgets[1];
    GtkWidget *subwindow = widgets[2];

    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(entry));
    const gchar *text = gtk_entry_buffer_get_text(buffer);
    gchar *output_text = g_strdup_printf("No duplicate files are deleted!!!");
    gtk_label_set_text(GTK_LABEL(label), output_text);
    g_free(output_text);
    gtk_window_destroy(GTK_WINDOW(subwindow));  //ta3ml destroy lel alert ki tenzel no

}



static void on_deleteButton_clicked(GtkWidget *button, gpointer user_data) {

    //Create alert subwindow
    GtkWidget *subwindow = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(subwindow), "ALERT !!!!");
    gtk_window_set_default_size(GTK_WINDOW(subwindow), 100, 100);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *sublabel = gtk_label_new("Do you really want to delete the duplicate files?");
    gtk_box_append(GTK_BOX(vbox), sublabel);

    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    // Create the Yes button
    GtkWidget *yes_button = gtk_button_new_with_label("Yes");
    gtk_box_append(GTK_BOX(button_box), yes_button);

    // Create the No button
    GtkWidget *no_button = gtk_button_new_with_label("No");
    gtk_box_append(GTK_BOX(button_box), no_button);

    //Center the Buttons
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(button_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vbox), button_box);

    gtk_window_set_child(GTK_WINDOW(subwindow), vbox);
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);

    GtkWidget **widgets = (GtkWidget **) user_data;
    widgets[2] = subwindow;


    g_signal_connect(yes_button, "clicked", G_CALLBACK(on_yes_button_clicked), user_data); //ki tenzel 3al button te5dem el function
    g_signal_connect(no_button, "clicked", G_CALLBACK(on_no_button_clicked), user_data);

    gtk_window_present(GTK_WINDOW(subwindow));  //Present the window
}


//tous ce qui est zina . En Fait enajmou nzidou code css lel gtk
static void apply_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider,
        "window { "
        "    background-color: #f5f5f5; "  
        "} "
        "entry { "
        "    color: #1a1a1a; "  
        "    background-color: #e6e6e6; "  
        "    border: 1px solid #bfbfbf; "  
        "    border-radius: 4px; "
        "    padding: 8px; "
        "    font-size: 14px; "
        "} "
        "entry:hover { "
        "    border-color: #a6a6a6; "  
        "} "
        "button { "
        "    background-color:rgb(49, 84, 240); "  
        "    color: #ffffff; "  
        "} "
        "button:hover { "
        "    background-color: #005a9e; "  
        "} "
        "label { "
        "    color: #1a1a1a; "  
        "    font-size: 15px; "
        "    font-weight: normal; "
        "} "
        "button:active { "
        "    background-color: #003e70; "  
        "} ");

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );
    g_object_unref(provider);
}





//activate the gtk program (En fait kima el main prog fel gtk)
static void activate(GtkApplication *app, gpointer user_data) {
#pragma region ||.........DEFINE VARIABLES.........||
    GtkWidget *window;
    GtkWidget *main_box;
    GtkWidget *top_box;
    GtkWidget *mid_box;
    GtkWidget *entry;
    GtkWidget *showFilesButton;
    GtkWidget *showDuplicateButton;
    GtkWidget *folderButton;
    GtkWidget *deleteButton;
    GtkWidget *output_label;
#pragma endregion

#pragma region ||.........INSTATIATE THE WIDGETS.........||
    //Main Window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "DUPLICATE X");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    //el box el da5leni
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_window_set_child(GTK_WINDOW(window), main_box);
    //el box win fama entry
    top_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append(GTK_BOX(main_box), top_box);
    //mid box (win fama el buttons wel output)
    mid_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append(GTK_BOX(main_box), mid_box);
    //entry (input text)
    entry = gtk_entry_new();
    gtk_widget_set_name(entry, "entry");
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_box_append(GTK_BOX(top_box), entry);
    //folder button
    folderButton = gtk_button_new_with_label("Select Folder");
    gtk_widget_set_name(folderButton, "folderbutton");
    gtk_box_append(GTK_BOX(top_box), folderButton);
    //button eli twari el files
    showFilesButton = gtk_button_new_with_label("Show Files");
    gtk_widget_set_name(showFilesButton, "showFilesButton");
    gtk_widget_set_hexpand(showFilesButton, TRUE);
    gtk_box_append(GTK_BOX(mid_box), showFilesButton);
    //button eli twari el duplicate files
    showDuplicateButton = gtk_button_new_with_label("Show Duplicate Files");
    gtk_widget_set_name(showDuplicateButton, "showDuplicateFilesButton");
    gtk_widget_set_hexpand(showDuplicateButton, TRUE);
    gtk_box_append(GTK_BOX(mid_box), showDuplicateButton);
    //Delete Button
    deleteButton = gtk_button_new_with_label("Delete Duplicates");
    gtk_widget_set_name(deleteButton, "deleteButton");
    gtk_widget_set_hexpand(deleteButton, TRUE);
    gtk_box_append(GTK_BOX(mid_box), deleteButton);
    //win bech isir el output
    output_label = gtk_label_new("Enter text above and press Submit.");
    gtk_widget_set_halign(output_label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(output_label, GTK_ALIGN_CENTER);
    gtk_widget_set_name(output_label, "label");
    gtk_box_append(GTK_BOX(main_box), output_label);
#pragma endregion
    

    GtkWidget **widgets = g_new(GtkWidget *, 2);
    widgets[0] = entry;
    widgets[1] = output_label;
#pragma region ||.........LISTEN FOR BUTTON CLICKS.........||
    //When Clicking on the button the function executes (showFilesButton <--- Button ||| on_showFilesButton_clicked <--- Function)
    g_signal_connect(showFilesButton, "clicked", G_CALLBACK(on_showFilesButton_clicked), widgets);
    g_signal_connect(showDuplicateButton, "clicked", G_CALLBACK(on_showDuplicateFilesButton_clicked), widgets);
    g_signal_connect(deleteButton, "clicked", G_CALLBACK(on_deleteButton_clicked), widgets);
#pragma endregion  

    apply_css(); //apply el art
    gtk_window_present(GTK_WINDOW(window)); //present el window
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example.enhancedinput", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
