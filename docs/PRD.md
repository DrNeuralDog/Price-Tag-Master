PriceTagMaster - Product Requirements Document (PRD)

Version: 0.1 (Draft)
Last Updated: 2025-08-08

1. Overview
   PriceTagMaster is a cross-platform desktop application built with C++ and Qt 6. Its primary purpose is to generate printable price tags from an input Microsoft Excel workbook according to a flexible, user-defined tag template, and export the result to either Microsoft Word (DOCX) or Microsoft Excel (XLSX). The input data format and parsing rules are defined in docs/OtherHelpfulFiles/ExcelAPI.txt.
2. Goals and Non-Goals
   2.1 Goals

- Generate price tags for A4 paper with automatic layout (tags per row, rows per page) based on tag dimensions and margins.
- Allow users to design and modify tag templates (visual appearance and dimensions) via an in-app editor with live preview.
- Import product data from Excel where column order is arbitrary and only column names are authoritative.
- Export composed sheets to DOCX and XLSX while preserving A4 page size and preventing clipping.
- Provide light and dark UI themes and support drag-and-drop for input files.
- Run on Windows and major Linux distributions; build with CMake; keep dependencies minimal; optional Conan usage.

2.2 Non-Goals

- Printing pipeline management beyond exporting A4-ready documents.
- Non-Microsoft formats for input or output.

3. Actors and Primary Flows

- Retail Operator: opens an Excel file, previews generated tags, tweaks template, exports to DOCX/XLSX for printing.
- Advanced User: creates multiple templates, toggles theme, adjusts margins/spacing, sets replication count per row.

Primary Flow (Happy Path)

1) Select or drag-and-drop an input Excel file.
2) App parses headers and rows per Excel API rules.
3) User opens Tag Template Editor, adjusts fields, fonts, sizes, margins.
4) Preview renders tag grid for A4; user validates layout and pagination.
5) User selects export format (DOCX or XLSX) and saves output.

4. Functional Requirements (AI-readable, MUST/SHOULD; IDs FR-*)

- FR-1 (Input): The app MUST accept .xlsx files as input; other input formats are out of scope.
- FR-2 (Parsing): The app MUST locate data by header names defined in ExcelAPI.txt; column order is arbitrary.
- FR-3 (Carry-Down): The app MUST carry down values for Supplier and Address to subsequent rows until overridden.
- FR-4 (Optional Fields): Empty cell values MUST be ignored (not rendered on tags).
- FR-5 (Missing Headers): If a required header is missing or renamed, its data MUST be ignored without error.
- FR-6 (Category+Gender Rule): If Category text length ≤ 12, combine Category and Gender into one line; otherwise ignore Gender.
- FR-7 (Replication): For a single input row, the app MUST support producing one or multiple identical tags as configured.
- FR-8 (Template Editor): Users MUST be able to change tag visual layout, visible fields, fonts, sizes, alignment, and tag width/height.
- FR-9 (Preview): A live preview MUST show a paginated A4 sheet with computed grid layout (tags per row, rows per page).
- FR-10 (A4 Constraint): Output page size MUST always be A4, regardless of export format.
- FR-11 (Layout Calculation): The engine MUST compute columns (N_cols) and rows (N_rows) fitting A4, considering margins and inter-tag spacing, ensuring no clipping.
- FR-12 (Export): The app MUST export the composed document to DOCX and XLSX preserving layout fidelity.
- FR-13 (Theming): The app MUST support light and dark themes.
- FR-14 (UX): The app MUST provide a file selector and a drag-and-drop area with a prominent plus button.
- FR-15 (Localization): The app SHOULD support UI text in English initially; future localization is out of scope.
- FR-16 (Validation): The app SHOULD validate input headers and report absent expected headers as informational notices.

5. Excel Data Contract (from ExcelAPI.txt)
   5.1 Expected Headers (exact, case-sensitive; language as in source):

- "Поставщик" (Supplier)
- "Адрес" (Address)
- "Фирма" (Brand)
- "Категория" (Category)
- "Пол" (Gender)
- "Страна бренда" (Brand Country)
- "Место производства" (Manufactured In)
- "Материал" (Material)
- "Размер" (Size)
- "Артикул" (SKU)
- "Цена" (Price)
- "Цена 2" (Price 2)
- "Количество" (Quantity)
- "Прочие данные" (Other Data) [zero or more columns with this name may appear]

5.2 Parsing Rules

- Column order is arbitrary. Only exact header names define semantics.
- Unrecognized or missing headers are ignored silently.
- Empty cells are ignored.
- Carry-down: "Поставщик" and "Адрес" values persist downward until overridden.
- Category+Gender concatenation rule: if length(Category) ≤ 12, render "Категория + Пол" on one line; else omit Gender.

6. Tag Layout and Pagination

- Input to layout engine: tag width/height (mm), page margins (mm), inter-tag spacing (mm), page size = A4 (210×297 mm) portrait.
- Computation: floor-based fit: N_cols = floor((210 - left - right + hSpacing) / (tagW + hSpacing)); N_rows = floor((297 - top - bottom + vSpacing) / (tagH + vSpacing)).
- Constraints: N_cols ≥ 1, N_rows ≥ 1; if not, prompt to reduce tag size or margins.
- Pagination: Fill rows left-to-right, top-to-bottom; replicate per-row tags as configured.

7. Template Editor Requirements

- Editable elements: text fields mapping to headers, static text blocks, images (optional later), borders, font, size, weight, alignment, padding.
- Units: mm for geometry; pt for fonts.
- Presets: provide starter templates; users can save/load templates (JSON).

8. Preview Requirements

- Real-time preview with current template and first N rows.
- Page navigator for multi-page preview.
- Zoom in/out; snap-to-grid optional.

9. Export Requirements

- DOCX export: A4 page, fixed layout grid; fonts embedded or standard.
- XLSX export: A4 print area, cell sizes approximating tag geometry; print scaling disabled to avoid distortion.
- Fidelity: Differences < 1 mm acceptable.

10. Platform, Build, Dependencies

- Platform: Windows 10+ and mainstream Linux distros (x86_64).
- Framework: C++17+, Qt 6.
- Build: CMake (out-of-source).
- Package Manager: Prefer no external deps; Conan optional. Third-party libraries for DOCX/XLSX are permitted if needed.

11. UX Requirements

- Light/Dark theme toggle.
- File chooser and drag-and-drop area with a plus-button.
- Non-blocking parsing with progress indicator; clear errors.

12. Error Handling & Reporting

- Missing expected headers: informational notice listing missing names.
- Malformed workbook: user-friendly error with remediation tips.
- Runtime exceptions: logged to a rotating log file; show minimal UI error banner.

13. Non-Functional Requirements

- Performance: Parse 5k rows x 20 columns < 3 seconds on mid-range PC.
- Memory: < 300 MB typical during preview of 5k tags.
- Reliability: No crashes with empty or partially filled columns.
- Testability: Deterministic layout calculation; unit tests for parsing and layout.

14. Acceptance Criteria (AC-*)

- AC-1: Given a valid Excel workbook with headers as per contract, preview shows correctly populated tags.
- AC-2: Changing tag width/height updates N_cols and N_rows accordingly, never causing clipping in export.
- AC-3: Category length ≤ 12 joins with Gender on one line; otherwise Gender is omitted.
- AC-4: Supplier and Address propagate downward until overridden.
- AC-5: Exported DOCX and XLSX are A4 and match preview within 1 mm tolerance.
- AC-6: Dark theme applies to all UI controls without layout glitches.

15. Glossary

- Tag: A single price label instance placed on a sheet.
- Template: A saved configuration describing tag geometry and mapped fields.
- Carry-down: Rule of inheriting previous non-empty cell values for designated columns.

16. Open Questions

- Should templates support barcodes/QR codes in v1? (Out of scope for now.)
- Do we need localization beyond English in v1?

17. Styles
    * When writing code, you must strictly follow the code style document: styles/DrunkDogStyle.clang-format
18. References

- Input contract: docs/OtherHelpfulFiles/ExcelAPI.txt
- Qt 6: https://doc.qt.io
- CMake: https://cmake.org
