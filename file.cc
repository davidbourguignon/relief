#include <gtk/gtk.h>
#include <fstream>

#include "application.hh"
#include "drawing.hh"
#include "meshing.hh"
#include "viewer.hh"
#include "file.hh"

using namespace std;

const char *File::_NULL_NAME = "";
const char *File::_DEFAULT_NAME = "untitled.rlf";
const string File::_RLF_EXTENSION = string(".rlf", 4);
const string File::_TR2_EXTENSION = string(".tr2", 4);
const string File::_TR3_EXTENSION = string(".tr3", 4);
const string File::_OFF_EXTENSION = string(".off", 4);

File::File(Application *application) {
  _application = application;
  _drawing_proxy = NULL;
  _meshing_proxy = NULL;
  _viewer_proxy = NULL;
  _file_selection = NULL;
  _name = _NULL_NAME;
  _name_selected = _NULL_NAME;
}

File::~File(void) {
  _application->remove(_drawing_proxy);
  _application->remove(_meshing_proxy);
  _application->remove(_viewer_proxy);
}

void
File::create(void) {
  _name = _DEFAULT_NAME;
  if ((_drawing_proxy = _application->drawing())->init() &&
      (_meshing_proxy = _application->meshing())->init()) {
    _viewer_proxy = _application->viewer();
  } else {
    _application->remove(this);
  }
}

static string
local_name(const string& global_name) {
#if defined(_WIN32)
  const string::value_type DIRECTORY_SEPARATOR = string("\\")[0];
#else
  const string::value_type DIRECTORY_SEPARATOR = string("/")[0];
#endif
  
  string local_name = "";
  if (!global_name.empty()) {
    string::size_type begin_pos
      = global_name.find_last_of(DIRECTORY_SEPARATOR) + 1;
    string::size_type end_pos
      = global_name.size();
    local_name = global_name.substr(begin_pos, end_pos);
  }
  return local_name;
}

void
File::open(Type type, const char *name) {
  gint response = GTK_RESPONSE_CANCEL;
  if (strcmp(name, _NULL_NAME)) {
    response = GTK_RESPONSE_OK;
    _name_selected = name;
  } else {
    switch (type) {
    case RLF:
      response = _create_file_selection("Open", "*.rlf");
      break;
    case OFF:
      response = _create_file_selection("Import OFF", "*.off");
      break;
    default:
      assert(false);
      break;
    }
  }
  if (response == GTK_RESPONSE_OK) {
    bool is_reading_failure = false;
    ifstream fin;
    fin.open(_name_selected.c_str());
    if (!fin.is_open()) {
      _display_close_message(GTK_MESSAGE_ERROR,
                             "Error: unable to open file %s for reading!\n",
                             _name_selected.c_str());
    } else {
      cout << "Reading file " << _name_selected << endl;
      switch (type) {
      case RLF: {
        string name_read;
        fin >> name_read;
        string name_read_d, name_read_m;
        fin >> name_read_d;
        fin >> name_read_m;
        ifstream fin_d, fin_m;
        fin_d.open(name_read_d.c_str());
        fin_m.open(name_read_m.c_str());
        if (fin_d.is_open() &&
            fin_m.is_open() &&
            (_drawing_proxy = _application->drawing())->read(fin_d, type) &&
            (_meshing_proxy = _application->meshing())->read(fin_m, type)) {
          _name = _name_selected;
          _viewer_proxy = _application->viewer();
        } else {
          is_reading_failure = true;
        }
      } break;
      case OFF:
        if ((_drawing_proxy = _application->drawing())->init() &&
            (_meshing_proxy = _application->meshing())->read(fin, type)) {
          _name = _name_selected;
          string::size_type begin_pos = _name.find(_OFF_EXTENSION);
          _name.replace(begin_pos, _OFF_EXTENSION.size(), _RLF_EXTENSION);
          _viewer_proxy = _application->viewer();
        } else {
          is_reading_failure = true;
        }
        break;
      default:
        assert(false);
        break;
      }
      if (fin.fail() || is_reading_failure) {
        _display_close_message(GTK_MESSAGE_ERROR,
                               "Error: while reading file %s!\n",
                               _name_selected.c_str());
        fin.clear();
      }
    }
    fin.close();
    if (is_reading_failure) {
      _application->remove(this);
    } else {
      cout << "done." << endl;
    }
  }
}

void
File::save(void) {
  if (_name == _DEFAULT_NAME) {
    save_as(RLF);
  } else {
    save_as(RLF, _name.c_str());
  }
}

void
File::save_as(Type type, const char *name) {
  gint response = GTK_RESPONSE_CANCEL;
  if (strcmp(name, _NULL_NAME)) {
    response = GTK_RESPONSE_OK;
    _name_selected = name;
  } else {
    switch (type) {
    case RLF:
      response = _create_file_selection("Save as", "*.rlf");
      break;
    case OFF:
      response = _create_file_selection("Export OFF", "*.off");
      break;
    default:
      assert(false);
      break;
    }
  }
  if (response == GTK_RESPONSE_OK) {
    bool is_writing_failure = false;
    ofstream fout;
    fout.open(_name_selected.c_str());
    if (!fout.is_open()) {
      _display_close_message(GTK_MESSAGE_ERROR,
                             "Error: unable to open file %s for writing!\n",
                             _name_selected.c_str());
    } else {
      cout << "Writing file " << _name_selected << endl;
      switch (type) {
      case RLF: {
        string name_written = _name_selected;
        string::size_type begin_pos = name_written.find(_RLF_EXTENSION);
        if (begin_pos == string::npos) {
          _display_close_message(GTK_MESSAGE_ERROR,
                                 "Error: file %s is not a RLF file!\n",
                                 name_written.c_str());
          break;
        }
        string name_written_d = name_written;
        name_written_d.replace(begin_pos, _RLF_EXTENSION.size(),
                               _TR2_EXTENSION);
        string name_written_m = name_written;
        name_written_m.replace(begin_pos, _RLF_EXTENSION.size(),
                               _TR3_EXTENSION);
        fout << name_written   << endl;
        fout << name_written_d << endl;
        fout << name_written_m << endl;
        ofstream fout_d, fout_m;
        fout_d.open(name_written_d.c_str());
        fout_m.open(name_written_m.c_str());
        if (fout_d.is_open() &&
            fout_m.is_open() &&
            _drawing_proxy->write(fout_d, type) &&
            _meshing_proxy->write(fout_m, type)) {
          _name = _name_selected;
          _viewer_proxy->sync();
        } else {
          is_writing_failure = true;
        }
      } break;
      case OFF:
        if (!_meshing_proxy->write(fout, type)) {
          is_writing_failure = true;
        }
        break;
      default:
        assert(false);
        break;
      }
      if (fout.fail() || is_writing_failure) {
        _display_close_message(GTK_MESSAGE_ERROR,
                               "Error: while writing file %s!\n",
                               _name_selected.c_str());
        fout.clear();
      }
    }
    fout.close();
    if (!is_writing_failure) {
      cout << "done." << endl;
    }
  }
}

bool
File::close(void) {
  bool is_closing = false;
  if ((_drawing_proxy != NULL && _drawing_proxy->has_changed()) ||
      (_meshing_proxy != NULL && _meshing_proxy->has_changed())) {
    gint response
      = _display_ok_cancel_message(GTK_MESSAGE_QUESTION,
                                   "Changes were made to %s.\nClose anyway?\n",
                                   _DEFAULT_NAME);
    if (response == GTK_RESPONSE_OK) {
      is_closing = true;
    }
  } else {
    is_closing = true;
  }
  if (is_closing) {
    _application->remove(this);
  }
  return is_closing;
}

const char *
File::name(void) {
  assert(_name != _NULL_NAME);
  return _name.c_str();
}

gint
File::_file_selection_cb(GtkWidget *widget, File *file) {
  file->_name_selected
    = gtk_file_selection_get_filename(
        GTK_FILE_SELECTION(file->_file_selection));
  return TRUE;
}

gint
File::_create_file_selection(const gchar *action, const gchar *extension) {
  GtkWidget *file_selection = gtk_file_selection_new(action);
  gtk_file_selection_complete(GTK_FILE_SELECTION(file_selection), extension);
  gtk_file_selection_set_select_multiple(GTK_FILE_SELECTION(file_selection),
                                         FALSE);
  g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(file_selection)->ok_button),
                   "clicked", G_CALLBACK(_file_selection_cb), this);
  g_signal_connect(G_OBJECT(file_selection), "delete_event",
                   G_CALLBACK(gtk_widget_destroy), file_selection);
  g_signal_connect(G_OBJECT(file_selection), "destroy",
                   G_CALLBACK(gtk_widget_destroyed), &_file_selection);
  gtk_widget_show(file_selection);
  _set_file_selection(file_selection);
  gint response = gtk_dialog_run(GTK_DIALOG(file_selection));
  assert(response == GTK_RESPONSE_OK          ||
         response == GTK_RESPONSE_CANCEL      ||
         response == GTK_RESPONSE_DELETE_EVENT);
  gtk_widget_destroy(file_selection);
  return response;
}

gint
File::_display_close_message(GtkMessageType type,
                             const gchar *message,
                             const gchar *name) const {
  GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                             GTK_DIALOG_MODAL,
                                             type,
                                             GTK_BUTTONS_CLOSE,
                                             message, name);
  gtk_widget_show(dialog);
  gint response = gtk_dialog_run(GTK_DIALOG(dialog));
  assert(response == GTK_RESPONSE_CLOSE       ||
         response == GTK_RESPONSE_DELETE_EVENT);
  gtk_widget_destroy(dialog);
  return response;
}

gint
File::_display_ok_cancel_message(GtkMessageType type,
                                 const gchar *message,
                                 const gchar *name) const {
  GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                             GTK_DIALOG_MODAL,
                                             type,
                                             GTK_BUTTONS_OK_CANCEL,
                                             message, name);
  gtk_widget_show(dialog);
  gint response = gtk_dialog_run(GTK_DIALOG(dialog));
  assert(response == GTK_RESPONSE_OK          ||
         response == GTK_RESPONSE_CANCEL      ||
         response == GTK_RESPONSE_DELETE_EVENT);
  gtk_widget_destroy(dialog);
  return response;
}

void
File::_set_file_selection(GtkWidget* file_selection) {
  if (GTK_IS_WIDGET(_file_selection)) {
    gtk_widget_destroy(_file_selection);
  }
  _file_selection = file_selection;
  assert(_file_selection != NULL);
}
