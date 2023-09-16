#include <iostream>
#include <gtk-3.0/gtk/gtk.h>
#include <sqlite3.h>

GtkWidget *win, *table1, *cal, *l1, *lb, *temp_l, *e1, *btn1, *btn2, *btn3, *dl, *cb, *cbt, *cbt2, *winh, *lbh, *scwinh, *tableh, *btnh, *btnh2, *btnh3, *ctl, *wina, *abtl, *table2, *abtl2;
GtkEntryBuffer *buff;
GtkListBoxRow *r1, *r2;
sqlite3 *db;
guint day, mt, yr;
int ext, iid = 1, count = 0;
bool empty = 0, pr = 1;
std::string res, dres;
char *msgerr;
std::string q, temp;
GtkStyleContext*cont;

void ext_date()  //extracting selected date from calendar widget
{
        gtk_calendar_get_date(GTK_CALENDAR(cal), &yr, &mt, &day);
        temp = std::to_string(day) + std::to_string(mt) + std::to_string(yr);
        res = "";
        for (int i = 0; i < temp.size(); i++)
        {
                if (temp[i] == '0') res += "zero";
                if (temp[i] == '1') res += "one";
                if (temp[i] == '2') res += "two";
                if (temp[i] == '3') res += "three";
                if (temp[i] == '4') res += "four";
                if (temp[i] == '5') res += "five";
                if (temp[i] == '6') res += "six";
                if (temp[i] == '7') res += "seven";
                if (temp[i] == '8') res += "eight";
                if (temp[i] == '9') res += "nine";
        }
}

void nr()  //naming of listboxrows for styling
{
        int index = 0;
        while (gtk_list_box_get_row_at_index(GTK_LIST_BOX(lbh), index) != NULL)
        {
                r1 = gtk_list_box_get_row_at_index(GTK_LIST_BOX(lbh), index++);
                gtk_widget_set_name((GtkWidget *)r1, "r1");
        }
}

void nr1() //naming of listboxrows of task history window for styling
{
        int index = 0;
        while (gtk_list_box_get_row_at_index(GTK_LIST_BOX(lb), index) != NULL)
        {
                r1 = gtk_list_box_get_row_at_index(GTK_LIST_BOX(lb), index++);
                gtk_widget_set_name((GtkWidget *)r1, "r1");
        }
}

void add_css_class(GtkWidget *w, const gchar *cn)
{
        cont = gtk_widget_get_style_context((GtkWidget *)w);
        gtk_style_context_add_class(cont, cn);
}

void sql_err(char *err) //displaying error if any sql command gives an error
{
        std::cout << err << "\n";
        sqlite3_free(err);
        return;
}

static int lb_ins(void *data, int argc, char **argv, char **azcolname)  //inserting rows in listbox
{
        for (int i = 0; i < argc; i++)
        {
                int index = 0;
                std::string s1 = (std::string)azcolname[i];
                if (s1 == "id")
                        continue;
                r1 = gtk_list_box_get_row_at_index(GTK_LIST_BOX(lb), index++);
                std::string t = (std::string)(argv[i] ? argv[i] : NULL);
                if (t == "0")
                        continue;
                if (t == "1")
                {
                        add_css_class((GtkWidget *)r1, "rcmplt");
                        continue;
                }
                if (t == "High")
                {
                        add_css_class((GtkWidget *)r1, "r2");
                        continue;
                }
                if (t == "Low")
                {
                        add_css_class((GtkWidget *)r1, "r3");
                        continue;
                }
                if (t == "Medium")
                {
                        add_css_class((GtkWidget *)r1, "r4");
                        continue;
                }
                const gchar *gc = (const gchar *)t.c_str();
                temp_l = gtk_label_new(gc);
                gtk_widget_set_name(temp_l, "temp_l");
                gtk_list_box_prepend(GTK_LIST_BOX(lb), temp_l);
                gtk_widget_show(temp_l);
                gtk_label_set_line_wrap(GTK_LABEL(temp_l), 1);
                gtk_label_set_line_wrap_mode(GTK_LABEL(temp_l), (PangoWrapMode)GTK_WRAP_WORD);
                gtk_widget_set_halign(temp_l, GTK_ALIGN_START);
        }
        return 0;
}

static int emp_table_check(void *data, int argc, char **argv, char **azcolname)  //check if the table is empty in database
{
        empty = 0;
        count = 0;
        std::string str = (std::string)(argv[0] ? argv[0] : NULL);
        int n = std::stoi(str);
        if (n == 0)
        {
                empty = 1;
                return 0;
        }
        count = n;
        return 0;
}

static void b1f(GtkWidget *w, gpointer data)  //adding task to the table in sql
{
        const gchar *inp = gtk_entry_get_text(GTK_ENTRY(data));
        std::string cmm = (std::string)inp;
        if (cmm == "Enter task" || cmm == "")
                return;
        ext_date();
        q = "select count(*) from " + res + ";";
        ext = sqlite3_open("todo.db", &db);
        ext = sqlite3_exec(db, q.c_str(), emp_table_check, 0, &msgerr);
        const gchar *cbinp = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(cbt));
        if (cbinp == NULL || (std::string)cbinp == "Priority")
                return;
        std::string cbstr = (std::string)cbinp;
        q = "insert into " + res + " (task, priority, complete) values ('" + cmm + "', '" + cbstr + "', 0);";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        temp_l = gtk_label_new(inp);
        gtk_list_box_insert(GTK_LIST_BOX(lb), temp_l, -1);
        gtk_widget_set_name(temp_l, "temp_l");
        gtk_widget_show(temp_l);
        gtk_label_set_line_wrap(GTK_LABEL(temp_l), 1);
        gtk_label_set_line_wrap_mode(GTK_LABEL(temp_l), (PangoWrapMode)GTK_WRAP_WORD);
        gtk_widget_set_halign(temp_l, GTK_ALIGN_START);
        sqlite3_close(db);
        gtk_entry_set_text(GTK_ENTRY(e1), "Enter task");
        if (!pr)
                nr1();
        int index = 0;
        while (gtk_list_box_get_row_at_index(GTK_LIST_BOX(lb), index) != NULL)
                r1 = gtk_list_box_get_row_at_index(GTK_LIST_BOX(lb), index++);
        inp = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(cbt));
        if ((std::string)inp == "High")
                add_css_class((GtkWidget *)r1, "r2");
        if ((std::string)inp == "Low")
                add_css_class((GtkWidget *)r1, "r3");
        if ((std::string)inp == "Medium")
                add_css_class((GtkWidget *)r1, "r4");
}

static void b2f(GtkWidget *w, gpointer data)  //deleting task from the table
// Since sqlite version used here doesn't supports dropping column directly so another table is used to copy the data without target column
{
        ext = sqlite3_open("todo.db", &db);
        r1 = gtk_list_box_get_selected_row(GTK_LIST_BOX(lb));
        if (r1 == NULL)
                return;
        int index = 0;
        index = (int)(gtk_list_box_row_get_index(r1));
        gtk_widget_destroy((GtkWidget *)r1);
        q = "delete from " + res + " where id = " + std::to_string(index + 1) + ";";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        q = "create table if not exists " + res + "c (id integer not null primary key, task varchar(40), priority varchar(8), complete integer);";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        q = "insert into " + res + "c (task, priority, complete) select task, priority, complete from " + res + ";";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        q = "delete from " + res + ";";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        q = "drop table " + res + ";";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        q = "alter table " + res + "c rename to " + res + ";";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        sqlite3_close(db);
}

static void b3f(GtkWidget *w, gpointer data) // deleting table and listboxrows
{
        sqlite3_open("todo.db", &db);
        ext_date();
        q = "drop table " + res + ";";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        q = "create table if not exists " + res + " (id integer not null primary key, task varchar(40), priority varchar(8), complete integer);";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        sqlite3_close(db);
        int index = 0;
        while (gtk_list_box_get_row_at_index(GTK_LIST_BOX(lb), index) != NULL)
        {
                r1 = gtk_list_box_get_row_at_index(GTK_LIST_BOX(lb), index);
                gtk_widget_destroy((GtkWidget *)r1);
        }
}


static void calf(GtkWidget *w, gpointer data)  //filling listbox after selecting date from calendar
{
        int index = 0;
        while (gtk_list_box_get_row_at_index(GTK_LIST_BOX(lb), index) != NULL)
        {
                r1 = gtk_list_box_get_row_at_index(GTK_LIST_BOX(lb), index);
                gtk_widget_destroy((GtkWidget *)r1);
        }
        gtk_calendar_get_date(GTK_CALENDAR(cal), &yr, &mt, &day);
        dres = std::to_string(day) + " ";
        temp = std::to_string(mt);
        if (temp == "0") dres += "January";
        if (temp == "1") dres += "February";
        if (temp == "2") dres += "March";
        if (temp == "3") dres += "April";
        if (temp == "4") dres += "May";
        if (temp == "5") dres += "June";
        if (temp == "6") dres += "July";
        if (temp == "7") dres += "August";
        if (temp == "8") dres += "September";
        if (temp == "9") dres += "October";
        if (temp == "10") dres += "November";
        if (temp == "11") dres += "December";
        dres += ", " + std::to_string(yr);
        ext_date();
        gtk_label_set_text(GTK_LABEL(dl), (const gchar *)dres.c_str());
        gtk_widget_set_halign(dl, GTK_ALIGN_CENTER);
        gtk_widget_show(dl);
        ext = sqlite3_open("todo.db", &db);
        q = "drop table if not exists " + res + "c;";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        q = "create table if not exists " + res + " (id integer primary key not null, task varchar(40), priority varchar(8), complete integer);";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        q = "select count(*) from " + res + ";";
        ext = sqlite3_exec(db, q.c_str(), emp_table_check, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        if (!empty)
        {
                q = "select * from " + res + " order by id desc;";
                ext = sqlite3_exec(db, q.c_str(), lb_ins, 0, &msgerr);
                if (ext != SQLITE_OK)
                        sql_err(msgerr);
        }
        sqlite3_close(db);
        if (!pr)
                nr1();
}

static void cmplt(GtkWidget *w, gpointer data) //setting a task as complete and adding it to the task history table
{
        r1 = gtk_list_box_get_selected_row(GTK_LIST_BOX(lb));
        if (r1 == NULL)
                return;
        add_css_class((GtkWidget *)r1, "rcmplt");
        ext_date();
        int index = 0;
        index = (int)(gtk_list_box_row_get_index(r1));
        ext = sqlite3_open("todo.db", &db);
        q = "update " + res + " set complete = 1 where id = " + std::to_string(index + 1) + ";";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        q = "create table if not exists " + res + "h (hid integer not null primary key, id integer not null unique, task varchar(40) not null UNIQUE, priority varchar(8), complete integer, UNIQUE(id, task));";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        q = "insert or replace into " + res + "h (id, task, priority, complete) select id, task, priority, complete from " + res + " where id = " + std::to_string(index + 1) + ";";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        sqlite3_close(db);
}

static void icmplt(GtkWidget *w, gpointer data) //undo task
{
        r1 = gtk_list_box_get_selected_row(GTK_LIST_BOX(lb));
        if (r1 == NULL)
                return;
        cont = gtk_widget_get_style_context((GtkWidget *)r1);
        gtk_style_context_remove_class(cont, "rcmplt");
        ext_date();
        int index = 0;
        index = (int)(gtk_list_box_row_get_index(r1));
        ext = sqlite3_open("todo.db", &db);
        q = "update " + res + " set complete = 0 where id = " + std::to_string(index + 1) + ";";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        q = "delete from " + res + "h where id = " + std::to_string(index + 1) + ";";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        sqlite3_close(db);
}

static int lb_insh(void *data, int argc, char **argv, char **azcolname) //inserting into task history listbox
{
        for (int i = 0; i < argc; i++)
        {
                int index = 0;
                std::string s1 = (std::string)azcolname[i];
                if (s1 == "id" || s1 == "priority" || s1 == "complete" || s1 == "hid")
                        continue;
                r1 = gtk_list_box_get_row_at_index(GTK_LIST_BOX(lbh), index++);
                std::string t = (std::string)(argv[i] ? argv[i] : NULL);
                const gchar *gc = (const gchar *)t.c_str();
                temp_l = gtk_label_new(gc);
                gtk_widget_set_name(temp_l, "temp_l");
                gtk_list_box_prepend(GTK_LIST_BOX(lbh), temp_l);
                gtk_widget_show(temp_l);
                gtk_label_set_line_wrap(GTK_LABEL(temp_l), 1);
                gtk_label_set_line_wrap_mode(GTK_LABEL(temp_l), (PangoWrapMode)GTK_WRAP_WORD);
                gtk_widget_set_halign(temp_l, GTK_ALIGN_START);
        }
        nr();
        return 0;
}

void static btnhf(GtkWidget *w, gpointer data) //deleting all records from task history table and listbox
{
        int index = 0;
        while (gtk_list_box_get_row_at_index(GTK_LIST_BOX(lbh), index != NULL))
        {
                r1 = gtk_list_box_get_row_at_index(GTK_LIST_BOX(lbh), index);
                gtk_widget_destroy((GtkWidget *)r1);
        }
        ext_date();
        sqlite3_open("todo.db", &db);
        q = "delete from " + res + "h;";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        sqlite3_close(db);
}

void static btnh2f(GtkWidget *w, gpointer data)  //refreshing listbox of task history
{
        int index = 0;
        while (gtk_list_box_get_row_at_index(GTK_LIST_BOX(lbh), index != NULL))
        {
                r1 = gtk_list_box_get_row_at_index(GTK_LIST_BOX(lbh), index);
                gtk_widget_destroy((GtkWidget *)r1);
        }
        ext = sqlite3_open("todo.db", &db);
        q = "select * from " + res + "h order by id desc;";
        ext = sqlite3_exec(db, q.c_str(), lb_insh, 0, &msgerr);
        gtk_widget_grab_focus(btnh);
        sqlite3_close(db);
}

void static btnh3f(GtkWidget *w, gpointer data)  //deleting record from task history table
{
        r1 = gtk_list_box_get_selected_row(GTK_LIST_BOX(lbh));
        int index = (int)gtk_list_box_row_get_index(r1);
        gtk_widget_destroy((GtkWidget *)r1);
        ext = sqlite3_open("todo.db", &db);
        q = "delete from " + res + "h where hid = " + std::to_string(index + 1);
        ext = sqlite3_exec(db, q.c_str(), lb_insh, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        q = "create table if not exists " + res + "t (hid integer not null primary key, id integer not null unique, task varchar(40) unique, priority varchar(8), complete integer);";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        q = "insert into " + res + "t (id, task, priority, complete) select id, task, priority, complete from " + res + "h;";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        q = "delete from " + res + "h;";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        q = "drop table " + res + "h;";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        q = "alter table " + res + "t rename to " + res + "h;";
        ext = sqlite3_exec(db, q.c_str(), NULL, 0, &msgerr);
        if (ext != SQLITE_OK)
                sql_err(msgerr);
        sqlite3_close(db);
}

static void tkht()  //creating task history window
{
        winh = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        lbh = gtk_list_box_new();
        tableh = gtk_table_new(8, 8, 0);
        scwinh = gtk_scrolled_window_new(NULL, NULL);
        btnh = gtk_button_new_with_label("Delete All");
        btnh2 = gtk_button_new_with_label("Refresh");
        btnh3 = gtk_button_new_with_label("Delete");
        ctl = gtk_label_new("Completed Task(s)");
        gtk_container_add(GTK_CONTAINER(scwinh), lbh);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scwinh), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
        gtk_table_attach(GTK_TABLE(tableh), ctl, 0, 8, 0, 1, GTK_EXPAND, GTK_FILL, 0, 0);
        gtk_table_attach(GTK_TABLE(tableh), scwinh, 0, 8, 1, 2, GTK_FILL, GTK_FILL, 20, 10);
        gtk_table_attach(GTK_TABLE(tableh), btnh, 2, 3, 3, 4, GTK_FILL, GTK_FILL, 0, 10);
        gtk_table_attach(GTK_TABLE(tableh), btnh3, 3, 4, 3, 4, GTK_FILL, GTK_FILL, 0, 10);
        gtk_table_attach(GTK_TABLE(tableh), btnh2, 4, 5, 3, 4, GTK_FILL, GTK_FILL, 0, 10);
        gtk_widget_set_size_request(scwinh, 600, 500);
        gtk_widget_set_halign(ctl, GTK_ALIGN_START);
        g_signal_connect(winh, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
        g_signal_connect(btnh, "clicked", G_CALLBACK(btnhf), NULL);
        g_signal_connect(btnh2, "clicked", G_CALLBACK(btnh2f), NULL);
        g_signal_connect(btnh3, "clicked", G_CALLBACK(btnh3f), NULL);
        ext = sqlite3_open("todo.db", &db);
        q = "select * from " + res + "h order by id desc;";
        ext = sqlite3_exec(db, q.c_str(), lb_insh, 0, &msgerr);
        gtk_widget_grab_focus(btnh);
        sqlite3_close(db);
        gtk_container_add(GTK_CONTAINER(winh), tableh);
        gtk_widget_show_all(winh);
        gtk_widget_set_name(winh, "winh");
        gtk_widget_set_name(btnh, "btnh");
        gtk_widget_set_name(btnh2, "btnh2");
        gtk_widget_set_name(btnh3, "btnh3");
        gtk_widget_set_name(scwinh, "scwinh");
        gtk_widget_set_name(lbh, "lbh");
        gtk_window_set_title(GTK_WINDOW(winh), "Task History");
        gtk_main();
}

static void abt() //showing about section for app in new window
{
        wina = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        abtl = gtk_label_new("ToDo App\nVersion: 0.1");
        abtl2 = gtk_label_new("Written By: Samanyu Vyas (samanyuvyas07@gmail.com)");
        table2 = gtk_table_new(4, 4, 0);
        gtk_table_attach(GTK_TABLE(table2), abtl, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 10, 10);
        gtk_table_attach(GTK_TABLE(table2), abtl2, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 10, 10);
        g_signal_connect(wina, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
        gtk_container_add(GTK_CONTAINER(wina), table2);
        gtk_widget_show_all(wina);
        gtk_window_set_title(GTK_WINDOW(wina) , "About ToDo App");
        gtk_widget_set_name(wina, "wina");
        gtk_main();
}

static void cbtf(GtkWidget *w, gpointer data) //defining combobox operation for its options
{
        const gchar *inp = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(cbt2));
        if ((std::string)inp == "Delete All")
                b3f(btn3, NULL);
        if ((std::string)inp == "Complete Task")
                cmplt(cbt2, NULL);
        if ((std::string)inp == "Task History")
        {
                ext_date();
                tkht();
        }
        if ((std::string)inp == "Undo Task")
                icmplt(cbt2, NULL);
        if ((std::string)inp == "Toggle Priority Visibility")
        {
                if (pr)
                {
                        pr = 0;
                        calf(NULL, NULL);
                }
                else
                {
                        pr = 1;
                        calf(NULL, NULL);
                }
        }
        if ((std::string)inp == "About")
                abt();
        gtk_combo_box_set_active(GTK_COMBO_BOX(cbt2), 0);
}

static void e1_clear(GtkWidget *w, gpointer data) //clearing task entry box
{
        gtk_entry_set_text(GTK_ENTRY(e1), "");
}

void addcss() //linking css file; css file and app executable should be in the same folder
{
        gtk_widget_set_name(cal, "cal");
        gtk_widget_set_name(lb, "lb");
        gtk_widget_set_name(l1, "l1");
        gtk_widget_set_name(dl, "dl");
        gtk_widget_set_name(win, "win");
        gtk_widget_set_name(btn1, "btn1");
        gtk_widget_set_name(btn2, "btn2");
        gtk_widget_set_name(btn3, "btn3");
        gtk_widget_set_name(e1, "e1");
        gtk_widget_set_name(cbt, "cbt");
        gtk_widget_set_name(cbt2, "cbt2");
        GtkCssProvider *prov1 = gtk_css_provider_new();
        gtk_css_provider_load_from_path(prov1, "todocss.css", NULL);
        gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(prov1), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

int main(int argc, char *argv[]) //main function; creating main window and its widgets
{
        if (ext)
                std::cout << "db error\n";
        gtk_init(&argc, &argv);
        win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        cal = gtk_calendar_new();
        l1 = gtk_label_new("Tasks:");
        dl = gtk_label_new("");
        lb = gtk_list_box_new();
        table1 = gtk_table_new(8, 9, 0);
        e1 = gtk_entry_new();
        btn1 = gtk_button_new_with_label("Add Task");
        btn2 = gtk_button_new_with_label("Remove Task");
        btn3 = gtk_button_new_with_label("Delete All");
        cbt = gtk_combo_box_text_new();
        cbt2 = gtk_combo_box_text_new();
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cbt), "Priority");
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cbt), "High");
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cbt), "Medium");
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cbt), "Low");
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cbt2), "Options");
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cbt2), "Task History");
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cbt2), "Complete Task");
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cbt2), "Undo Task");
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cbt2), "Delete All");
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cbt2), "Toggle Priority Visibility");
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cbt2), "About");
        gtk_combo_box_set_active(GTK_COMBO_BOX(cbt), 0);
        gtk_combo_box_set_active(GTK_COMBO_BOX(cbt2), 0);
        addcss();
        gtk_entry_set_buffer(GTK_ENTRY(e1), buff);
        gtk_entry_set_text(GTK_ENTRY(e1) ,"Enter task");
        gtk_table_attach(GTK_TABLE(table1), cal, 0, 8, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
        gtk_table_attach(GTK_TABLE(table1), e1, 0, 6, 7, 8, GTK_FILL, GTK_FILL, 10, 10);
        gtk_table_attach(GTK_TABLE(table1), btn1, 7, 8, 7, 8, GTK_FILL, GTK_FILL, 0, 10);
        gtk_table_attach(GTK_TABLE(table1), btn2, 1, 7, 8, 9, GTK_FILL, GTK_FILL, 20, 10);
        gtk_table_attach(GTK_TABLE(table1), l1, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
        gtk_table_attach(GTK_TABLE(table1), dl, 0, 8, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
        gtk_table_attach(GTK_TABLE(table1), cbt2, 7, 8, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
        gtk_table_attach(GTK_TABLE(table1), cbt, 6, 7, 7, 8, GTK_FILL, GTK_FILL, 0, 10);
        g_signal_connect(win, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
        g_signal_connect(cal, "day-selected-double-click", G_CALLBACK(calf), NULL);
        g_signal_connect(btn1, "clicked", G_CALLBACK(b1f), e1);
        g_signal_connect(btn2, "clicked", G_CALLBACK(b2f), NULL);
        g_signal_connect(btn3, "clicked", G_CALLBACK(b3f), NULL);
        g_signal_connect(e1, "grab-focus", G_CALLBACK(e1_clear), NULL);
        g_signal_connect(cbt2, "changed", G_CALLBACK(cbtf), NULL);
        gtk_container_add(GTK_CONTAINER(win), table1);
        GtkWidget *scwin = gtk_scrolled_window_new(NULL, NULL);
        gtk_container_add(GTK_CONTAINER(scwin), lb);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scwin), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
        gtk_table_attach(GTK_TABLE(table1), scwin, 0, 8, 3, 6, GTK_FILL, GTK_FILL, 20, 10);
        gtk_widget_set_name(scwin, "scwin");
        gtk_widget_set_size_request(scwin, 100, 500);
        gtk_widget_show_all(win);
        calf(NULL, NULL);
        gtk_widget_set_visible(btn2, 1);
        int wid, hei;
        gtk_window_get_size(GTK_WINDOW(win), &wid, &hei);
        gtk_window_set_title(GTK_WINDOW(win), "ToDo App");
        gtk_main();
        sqlite3_close(db);
}