#include <iostream>
#include <string>
#include <bitset>
#include <gtk/gtk.h>

// Логика перевода: Текст -> Бинарный код
std::string TextToBinary(const std::string& input) {
    std::string result = "";
    for (unsigned char c : input) {
        std::bitset<8> bits(c);
        result += bits.to_string() + " ";
    }
    return result;
}

// Логика перевода: Бинарный код -> Текст
std::string BinaryToText(const std::string& input) {
    std::string cleanInput = "";
    for (char c : input) {
        if (c == '0' || c == '1') cleanInput += c;
    }
    std::string result = "";
    for (size_t i = 0; i < cleanInput.length(); i += 8) {
        if (i + 8 <= cleanInput.length()) {
            std::string byteString = cleanInput.substr(i, 8);
            unsigned long byteVal = std::bitset<8>(byteString).to_ulong();
            result += static_cast<char>(byteVal);
        }
    }
    return result;
}

GtkWidget *entry_input, *entry_output, *radio_t2b;

// Обработчик кнопки конвертации
void on_convert_clicked(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *input_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(entry_input));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(input_buffer, &start, &end);
    char *input_text = gtk_text_buffer_get_text(input_buffer, &start, &end, FALSE);

    std::string result = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_t2b))) ? TextToBinary(input_text) : BinaryToText(input_text);

    GtkTextBuffer *output_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(entry_output));
    gtk_text_buffer_set_text(output_buffer, result.c_str(), -1);
    g_free(input_text);
}

// Обработчик кнопки копирования
void on_copy_clicked(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *output_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(entry_output));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(output_buffer, &start, &end);
    char *output_text = gtk_text_buffer_get_text(output_buffer, &start, &end, FALSE);

    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_text(clipboard, output_text, -1);
    g_free(output_text);
}

// Применение стилей CSS (Темная тема)
void apply_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    const gchar *css =
    "window { background-color: #1e1e24; color: #f5f5f5; font-family: 'Ubuntu', sans-serif; font-size: 14px; }"
    "label { color: #b0b3b8; font-weight: bold; }"
    "textview text { background-color: #2a2a32; color: #ffffff; border-radius: 6px; border: 1px solid #40404f; padding: 5px; }"
    "button.suggested-action { background-image: linear-gradient(to bottom, #4f46e5, #4338ca); color: white; font-weight: bold; border-radius: 6px; border: none; }"
    "button.suggested-action:hover { background-image: linear-gradient(to bottom, #6366f1, #4f46e5); }"
    "button.secondary-action { background-color: #40404f; color: white; border-radius: 6px; border: 1px solid #5a5a6e; }"
    "button.secondary-action:hover { background-color: #4f46e5; }"
    "radio { color: #f5f5f5; }";

    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    g_object_unref(provider);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    apply_css();

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Universal Binary Converter (Linux)");
    gtk_window_set_default_size(GTK_WINDOW(window), 480, 440);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *lbl1 = gtk_label_new("Enter text or binary code:");
    gtk_widget_set_halign(lbl1, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), lbl1, FALSE, FALSE, 0);

    entry_input = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(entry_input), GTK_WRAP_WORD);
    GtkWidget *scroll1 = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scroll1, -1, 90);
    gtk_container_add(GTK_CONTAINER(scroll1), entry_input);
    gtk_box_pack_start(GTK_BOX(vbox), scroll1, FALSE, FALSE, 0);

    radio_t2b = gtk_radio_button_new_with_label(NULL, "Text ➔ Binary");
    GtkWidget *radio_b2t = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_t2b), "Binary ➔ Text");
    GtkWidget *hbox_mode = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 25);
    gtk_box_pack_start(GTK_BOX(hbox_mode), radio_t2b, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_mode), radio_b2t, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_mode, FALSE, FALSE, 0);

    GtkWidget *hbox_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    GtkWidget *btn_convert = gtk_button_new_with_label("Convert");
    gtk_style_context_add_class(gtk_widget_get_style_context(btn_convert), "suggested-action");
    gtk_widget_set_size_request(btn_convert, 300, 42);
    g_signal_connect(btn_convert, "clicked", G_CALLBACK(on_convert_clicked), NULL);

    GtkWidget *btn_copy = gtk_button_new_with_label("Copy Result");
    gtk_style_context_add_class(gtk_widget_get_style_context(btn_copy), "secondary-action");
    g_signal_connect(btn_copy, "clicked", G_CALLBACK(on_copy_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(hbox_buttons), btn_convert, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), btn_copy, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_buttons, FALSE, FALSE, 0);

    GtkWidget *lbl2 = gtk_label_new("Result:");
    gtk_widget_set_halign(lbl2, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), lbl2, FALSE, FALSE, 0);

    entry_output = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(entry_output), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(entry_output), GTK_WRAP_WORD);
    GtkWidget *scroll2 = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), scroll2, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(scroll2), entry_output);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
