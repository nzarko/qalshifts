#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif
#include <QStyleOptionViewItem>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QPainter>

#include "printview.h"

PrintView::PrintView()
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    QStandardItemModel imodel(50,9);
    this->setModel(&imodel);
    setShowGrid(true);
    setGridStyle(Qt::SolidLine);
    horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    setSpan(0,0,1,9);
    setSpan(1,0,3,2);
    setSpan(4,2,1,7);
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

void PrintView::print(QPrinter *p)
{
//#ifndef QT_NO_PRINTER
//    resize(printer->width(), printer->height());
//    render(printer);
//#endif

    QPrinter printer(QPrinter::ScreenResolution);
    auto rows = model()->rowCount(QModelIndex());
    auto cols = model()->columnCount(QModelIndex());

    auto totalWidth = 0.0;
    for(auto i=0; i < cols; ++i)
        totalWidth += columnWidth(i);

    QPainter painter;
    painter.begin(&printer);

    auto pageSize = printer.pageRect();
    auto pageHeight = pageSize.height();

    auto scaleX = pageSize.width() / (totalWidth);
    painter.scale(scaleX, 0.9);

    auto totalHeight = 0.0 ;

    bool newPage = false;
    auto offsetY = 0;

    for(auto row =0; row <rows; ++row) {
        if(row > 5 && (model()->data(model()->index(row,0)).toString() == ""))
            continue;
        auto height = rowHeight(row);
        totalHeight += height;
        if( totalHeight > pageHeight) {
            totalHeight = height ;
            printer.newPage();
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
