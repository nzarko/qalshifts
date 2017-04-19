#include <QHeaderView>
#include <QStandardItemModel>
#include <QPainter>

#include "stuffprintview.h"

StuffPrintView::StuffPrintView()
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    QStandardItemModel imodel(105,15);
    this->setModel(&imodel);
    setShowGrid(true);
    setGridStyle(Qt::SolidLine);
    init();
}

void StuffPrintView::print(QPrinter *p)
{
    QPrinter *printer = p;
    auto rows = model()->rowCount(QModelIndex());
    auto cols = model()->columnCount(QModelIndex());

    auto totalWidth = 0.0;
    for(auto i=0; i < cols; ++i)
        totalWidth += columnWidth(i);

    QPainter painter;
    painter.begin(printer);

    auto pageSize = printer->pageRect();
    auto pageHeight = pageSize.height();

    auto scaleX = pageSize.width() / (totalWidth);
    painter.scale(scaleX, 0.9);

    auto totalHeight = 0.0 ;

    bool newPage = false;
    auto offsetY = 0;

    for(auto row =0; row <rows; ++row) {
//        if(row > 5 && (model()->data(model()->index(row,0)).toString() == ""))
//            continue;
        auto height = rowHeight(row);
        totalHeight += height;
        if( totalHeight > pageHeight) {
            totalHeight = height ;
            printer->newPage();
            newPage = true;
        }
        for (auto col=0; col < cols; ++col) {
            auto idx = model()->index(row, col);
            auto option = viewOptions();
            option.rect = visualRect(idx);

            if (newPage) {
                offsetY = option.rect.y();
                newPage = false;
            }
            option.rect.moveTop(option.rect.y()-offsetY);
            itemDelegate()->paint(&painter, option, idx);
        }
    }
    painter.end();
}

void StuffPrintView::init()
{
    startRow = 2;
    createSkeleton(false);
    createSkeleton(true);
    QString css = "QTableView::item { \
            border-left: 1px solid black; \
            border-top: 1px solid black; \
            }  \
            PrintView { \
            border-bottom: 1px solid black; \
            border-right: 1px solid black; \
            }";
    setStyleSheet(css);
}

void StuffPrintView::createSkeleton(bool forFuelEmployees)
{
    QStringList lst;
    lst << "ΗΜΕΡΑ" << "ΩΡΑΡΙΟ" << "ΩΡΕΣ" ;
    setSpan(startRow - 2,0,2,1);
    setSpan(startRow - 2,1,2,2);


    setSpan(startRow - 2,3,2,1);


    setSpan(startRow-2,4,1,6);



    lst.clear();
    lst << "ΔΕΥΤΕΡΑ" << "ΤΡΙΤΗ" << "ΤΕΤΑΡΤΗ"
        <<"ΠΕΜΠΤΗ" << "ΠΑΡΑΣΚΕΥΗ" <<"ΣΑΒΒΑΤΟ"
       <<"ΚΥΡΙΑΚΗ";
    QStringList hour_list, weekend_hour_list;
    int span = 1;
    if(forFuelEmployees) {
    hour_list << "06:30" << "14:30" << "8.0"
              << "08:00" << "13:00" << "5.0"
              << "17:00" << "20:00" << "3.0"
              << "14:30" << "22:00" << "7.5";
    weekend_hour_list << "07:00" << "14:30" << "7.5"
                      << "08:00" << "13:00" << "5.0"
                      << "17:00" << "20:00" << "3.0"
                        << "14:30" << "21:00" << "7.5"
                        <<"07:00" << "14:00" << "7.0"
                       <<"14:00" << "21:00" <<"7.0";
    span = 4;
    } else {
        hour_list << "06:30" << "14:30" << "8.0"
                  << "14:30" << "22:00" << "7.5";
        weekend_hour_list << "07:00" << "14:30" << "7.5"
                            << "14:30" << "21:00" << "7.5"
                            <<"07:00" << "14:00" << "7.0"
                           <<"14:00" << "21:00" <<"7.0";
        span = 2;
    }
    int k = 0;
    int r = startRow;
    while (k < lst.size()-2) {
        setSpan(r,0,span,1);
        ++k;
        r += rowSpan(r,0) + 1;
    }
    //Special Treatment for the weekend.
    //k-=1;
    while (k < lst.size()) {
        if(k==6) span = 2;
        setSpan(r,0,span,1);

        ++k;
        r += rowSpan(r,0) + 1;
    }

    lst.clear();
    lst << "Α/Α" << "ΟΝΟΜΑ" << "ΩΡΕΣ" << " " << " "
        << "ΠΑΡΑΤΗΡΗΣΕΙΣ"
        << "Ημερ. Ρεπό\nΤελ. Κυριακής"
        << "Ημερ. Τελ.\nΚυριακής"
        << "Διαφορά";

    startRow = r + 15;
}

