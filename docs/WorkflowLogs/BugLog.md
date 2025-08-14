[2025-08-09 10:35:00] build error: Use of undeclared identifier 'refreshPreviewButton' in `src/mainwindow.cpp:227` - Removed obsolete references after replacing Preview tab with Template Editor; build ok
[2025-08-09 10:45:00] link error: undefined reference to `TemplateEditorWidget::TemplateEditorWidget(QWidget*)` from `TemplateEditorDialog` - Ensured `src/templateeditor.cpp` is linked via `target_sources` in `CMakeLists.txt`; reconfigure required
[2025-08-09 10:55:00] runtime crash: opening Template Editor caused crash - Root cause: uninitialized `templateEditorDialog` pointer; Fix: default-initialize to nullptr in `include/mainwindow.h` and guard creation on click - Fixed

