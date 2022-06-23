#include <gtk/gtk.h>
#include <err.h>

#include "CLImain.h"

/*
#include "base_structures.h" 
#include "CLImain.h"
#include "letter_extraction.h"
#include "main_lib.h"
#include "main_pre_processing.h"
#include "pixel_operations.h"
#include "pre_processing.h"
#include "segmentation.h"

#include <stdio.h>
#include <stdlib.h>
#include "SDL/SDL_ttf.h"
#include "SDL/SDL.h"
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pwd.h>
#include <err.h>
#include <math.h>
#include <ctype.h>
*/

// Structure of the game.
typedef struct GUI
{
    GtkWindow* window;              // Main window
    GtkButton* load_button;         // Load button
    GtkButton* convert_button;       // Convert button
    GtkButton* save_button;         // Save button
    GtkButton* step_button;         // Step Button
		GtkWidget* image;               // place where we load image
		GtkWidget* textview;            // display location of text
		GtkTextBuffer* textbuffer;      // text buffer
		int* step_counter;              // Step Counter
		char *filename;
} GUI;

//Load button clicked 
void on_button_load_clicked(GtkButton *b, gpointer user_data)
{
	b = b;
	GUI *gui = user_data;

	//opens filechooser 
	GtkWidget *dialog; 
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN; 
	gint res;  

	dialog = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW(gui->window),
             action, ("_Cancel"), GTK_RESPONSE_CANCEL,
             ("_Open"), GTK_RESPONSE_ACCEPT, NULL);   

	res = gtk_dialog_run (GTK_DIALOG (dialog)); 
	
	//choose file

	//TO DO : OPEN and CANCEL button 

	if (res == GTK_RESPONSE_ACCEPT)
  	{	
			//get filename
			char *filename;
    	GtkFileChooser *filechooser = GTK_FILE_CHOOSER (dialog);
    	filename = gtk_file_chooser_get_filename (filechooser);
    	//load image 
			gtk_image_set_from_file(GTK_IMAGE(gui->image), filename); 
			gui->filename = filename;
    	//g_free (filename); 

			//gtk_widget_destroy(filechooser);
  	}
	

	gtk_text_buffer_set_text(gui->textbuffer, "", -1);
	*gui->step_counter = -1;
	gtk_widget_set_sensitive(GTK_WIDGET(gui->step_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(gui->convert_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(gui->save_button), FALSE);

	//close filechooser 
	gtk_widget_destroy(dialog);
}

void load_to_text(gpointer user_data)
{
		GUI *gui = user_data;

    char *file_name = "./result.txt";           
    char *file_contents = NULL;  

    g_file_get_contents(file_name, &file_contents, NULL, NULL);

		if (file_contents == NULL)
			file_contents = "No Text Found.";

		// Put the contents of the file into the GtkTextBuffer
    gtk_text_buffer_set_text(gui->textbuffer, file_contents, -1);

		gtk_widget_set_sensitive(GTK_WIDGET(gui->save_button), TRUE);

    //g_free(file_contents);
}

void load_intermediate_image(gpointer user_data)
{
	GUI *gui = user_data;
	gui->filename = "./processed.bmp";
	gtk_image_set_from_file(GTK_IMAGE(gui->image), gui->filename);
}

//Convert button clicked   
void on_button_convert_clicked(GtkButton *b, gpointer user_data)
{
	b = b;
	GUI *gui = user_data;

	char *call[] = {"./main", gui->filename, "g", "c", "n", "d", "s"};
	CLImain(7, call);

	gtk_widget_set_sensitive(GTK_WIDGET(gui->step_button), FALSE);

	load_intermediate_image(gui);

	load_to_text(gui);

	gtk_widget_set_sensitive(GTK_WIDGET(gui->convert_button), FALSE);
} 


//Step button clicked  
void on_button_step_clicked(GtkButton *b, gpointer user_data)
{
	b = b;
	GUI *gui = user_data;

	*gui->step_counter += 1;
	if (*gui->step_counter == 0)
    gtk_widget_set_sensitive(GTK_WIDGET(gui->convert_button), FALSE);

	char *letter;
	switch (*gui->step_counter)
	{
		case 0:
		  letter = "g";
		  break;
		case 1:
		  letter = "c";
		  break;
		case 2:
		  letter = "n";
		  break;
		case 3:
		  letter = "d";
		  break;
		case 4:
		  letter = "s";
		  break;
		default:
		  letter = "h";
		  break;
  } 

	char *call[] = {"./main", gui->filename, letter};
	CLImain(3, call);

	if (*gui->step_counter == 4) {
		load_to_text(gui);
		gtk_widget_set_sensitive(GTK_WIDGET(gui->step_button), FALSE);
	}

  load_intermediate_image(gui);
}

size_t len_str(char s[]) {
	size_t i = 0;
	while (s[i] != 0)
		i++;
	return i;
}

char *concat(char *str1, char *str2) {
	size_t size = len_str(str1) + len_str(str2) + 1;
	char *str = malloc(size * sizeof(char));
	if (str == NULL)
		errx(1, "Not enough memory!");

	char *p = str;
	while (*str1 != 0)
		*(p++) = *(str1++);
	while (*str2 != 0)
		*(p++) = *(str2++);
	*p = 0;

	return str;
}

//Save Button clicked 
void on_button_save_clicked(GtkButton *b, gpointer user_data)
{
		b = b;
  	GUI *gui = user_data; 

		GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE; 

    dialog = gtk_file_chooser_dialog_new ("Save File", GTK_WINDOW(gui->window),
             action, ("Cancel"), GTK_RESPONSE_CANCEL,
             ("Save"), GTK_RESPONSE_ACCEPT, NULL); 

    GtkFileChooser *filechooser = GTK_FILE_CHOOSER (dialog);

    gtk_dialog_run (GTK_DIALOG (dialog)); 

    char *filename;
    char *text;
    GtkTextIter start;
		GtkTextIter end;

		filename = gtk_file_chooser_get_filename (filechooser); 
		gtk_text_buffer_get_bounds (gui->textbuffer, &start, &end);
		text = gtk_text_buffer_get_text (gui->textbuffer, &start, &end, FALSE);
		
		char *new_filename = concat(filename, ".txt");

		g_file_set_contents (new_filename, text, -1, NULL);   
		
		//g_free (text);
		gtk_widget_destroy(dialog);
} 

void on_window_destroy()
{ gtk_main_quit(); } 

int main(int argc, char **argv) 
{  	
	if (argc > 1)
		return CLImain(argc, argv);

	//INIT 
	gtk_init(NULL, NULL);

  //INIT BUILDER 
	GtkBuilder* builder = gtk_builder_new ();

  GError* error = NULL;
  if (gtk_builder_add_from_file(builder, "GUI.glade", &error) == 0)
  {
  	g_printerr("Error loading file: %s\n", error->message);
    g_clear_error(&error);
    return 1;
  }
	
	//GtkBuilder *builder = gtk_builder_new();
	//gtk_builder_add_from_file(builder, "GUI.glade", NULL);
		
	//POINTERS 
	//GtkBuilder	 *builder; 
	GtkWindow	 *window; 
	GtkButton 	 *button_load;
	GtkButton	 *button_convert; 
	GtkButton 	 *button_save;   
	GtkButton	 *button_step	; 
	GtkWidget 	 *image;
	GtkWidget	 *textview;
	GtkTextBuffer *textbuffer;   

	gint step_counter = -1;
	gint *step_p = &step_counter;   


	//BUILDER 
  window = GTK_WINDOW(gtk_builder_get_object(builder, "window")); 
  button_load = GTK_BUTTON(gtk_builder_get_object(builder, "button_load")); 
  button_convert = GTK_BUTTON(gtk_builder_get_object(builder, "button_convert"));
  button_save = GTK_BUTTON(gtk_builder_get_object(builder, "button_save")); 
	button_step =  GTK_BUTTON(gtk_builder_get_object(builder, "button_step"));

  image = GTK_WIDGET(gtk_builder_get_object(builder, "image"));  
	textview = GTK_WIDGET(gtk_builder_get_object(builder, "textview"));
	textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "textbuffer")); 
 		
	GUI gui =
	{
		.window = window,
		.load_button = button_load,
		.convert_button = button_convert,
		.save_button = button_save,
		.step_button = button_step,
		.image = image,
		.textview = textview,
		.textbuffer = textbuffer,
		.step_counter = step_p,
		.filename = "",
	};
	gui = gui;

	// Disable Buttons
	gtk_widget_set_sensitive(GTK_WIDGET(gui.step_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(gui.convert_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(gui.save_button), FALSE);

	//CONNECT OBJECT
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), &gui);
	g_signal_connect(button_load, "clicked", G_CALLBACK(on_button_load_clicked), &gui);
	g_signal_connect(button_convert, "clicked", G_CALLBACK(on_button_convert_clicked), &gui); 
	g_signal_connect(button_save, "clicked", G_CALLBACK(on_button_save_clicked), &gui); 
	g_signal_connect(button_step, "clicked", G_CALLBACK(on_button_step_clicked), &gui); 
	
	gtk_builder_connect_signals(builder, &gui); 
  g_object_unref(builder); 
  gtk_widget_show_all(GTK_WIDGET(window)); 

	gtk_main();

	return 0;
}
