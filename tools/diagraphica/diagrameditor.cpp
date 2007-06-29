//  Copyright 2007 A.j. (Hannes) pretorius. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./diagrameditor.cpp

// --- diagrameditor.cpp --------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------


#include "diagrameditor.h"


// -- constructors and destructor -----------------------------------


// --------------------------
DiagramEditor::DiagramEditor(
    Mediator* m,
    Graph* g,
    GLCanvas* c )
    : Visualizer( m, g, c )
// --------------------------
{
    //setClearColor( 0.44, 0.59, 0.85 );
    setClearColor( 0.65, 0.79, 0.94 );

    diagram     = new Diagram( m/*, c*/ );
    editMode    = EDIT_MODE_SELECT;
    drgBegIdx1 = -1;
    drgBegIdx2 = -1;
    
    initMouse();

    clipBoardShape = NULL;
}


// ----------------------------
DiagramEditor::~DiagramEditor()
// ----------------------------
{
    if ( diagram != NULL )
    {
        delete diagram;
        diagram = NULL;
    }

    if ( clipBoardShape != NULL )
    {
        delete clipBoardShape;
        clipBoardShape = NULL;
    }
}


// -- set data functions --------------------------------------------


// --------------------------------------------
void DiagramEditor::setDiagram( Diagram* dgrm )
// --------------------------------------------
{
    delete diagram;
    diagram = dgrm;
}


// ------------------------------------
void DiagramEditor::setEditModeSelect()
// ------------------------------------
{
    editMode = EDIT_MODE_SELECT;
    
    for ( int i = 0; i < diagram->getSizeShapes(); ++i )
    {
        if ( diagram->getShape( i )->getMode() != Shape::MODE_NORMAL )
            diagram->getShape( i )->setModeEdit();
    }

    canvas->Refresh();
}


// ---------------------------------
void DiagramEditor::setEditModeDOF()
// ---------------------------------
{
    editMode = EDIT_MODE_DOF;
    deselectAll();
    canvas->Refresh();
}


// ----------------------------------
void DiagramEditor::setEditModeRect()
// ----------------------------------
{
    editMode = EDIT_MODE_RECT;
    deselectAll();
    canvas->Refresh();
}


// -------------------------------------
void DiagramEditor::setEditModeEllipse()
// -------------------------------------
{
    editMode = EDIT_MODE_ELLIPSE;
    deselectAll();
    canvas->Refresh();
}


// ----------------------------------
void DiagramEditor::setEditModeLine()
// ----------------------------------
{
    editMode = EDIT_MODE_LINE;
    deselectAll();
    canvas->Refresh();
}


// -----------------------------------
void DiagramEditor::setEditModeArrow()
// -----------------------------------
{
    editMode = EDIT_MODE_ARROW;
    deselectAll();
    canvas->Refresh();
}


// ------------------------------------
void DiagramEditor::setEditModeDArrow()
// ------------------------------------
{
    editMode = EDIT_MODE_DARROW;
    deselectAll();
    canvas->Refresh();
}


// ------------------------------------------------
void DiagramEditor::setShowGrid( const bool &flag )
// ------------------------------------------------
{
    diagram->setShowGrid( flag );
    canvas->Refresh();
}


// ------------------------------------------------
void DiagramEditor::setSnapGrid( const bool &flag )
// ------------------------------------------------
{
    diagram->setSnapGrid( flag );
    canvas->Refresh();
}


// -----------------------------
void DiagramEditor::setFillCol()
// -----------------------------
{
    Shape* s = NULL;
    
    int sizeShapes = diagram->getSizeShapes();
    for ( int i = 0; i < sizeShapes; ++i )    
    {
        if ( diagram->getShape( i )->getMode() == Shape::MODE_EDIT )
        {
            s = diagram->getShape( i );
            break;
        }
    }

    if ( s != NULL )
    {
        ColorRGB col;
        s->getFillColor( col );

        mediator->getColor( col );
        s->setFillColor( col );

        canvas->Refresh();
    }
     
    s = NULL;
}


// -----------------------------
void DiagramEditor::setLineCol()
// -----------------------------
{
    Shape* s = NULL;
    
    int sizeShapes = diagram->getSizeShapes();
    for ( int i = 0; i < sizeShapes; ++i )    
    {
        if ( diagram->getShape( i )->getMode() == Shape::MODE_EDIT )
        {
            s = diagram->getShape( i );
            break;
        }
    }

    if ( s != NULL )
    {
        ColorRGB col;
        s->getLineColor( col );
        
        mediator->getColor( col );
        s->setLineColor( col );

        canvas->Refresh();
    }

    s = NULL;
}


// --------------------------------------------------
void DiagramEditor::handleDOFSel( const int &DOFIdx )
// --------------------------------------------------
{
    Shape* s = NULL;

    int sizeShapes = diagram->getSizeShapes();
    for ( int i = 0; i < sizeShapes; ++i )    
    {
        if ( diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_XCTR || 
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_YCTR ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_HGT  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_WTH  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_AGL  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_COL  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_OPA )
        {
            s = diagram->getShape( i );
            break;
        }
    }

    if ( s != NULL )
    {
        if ( DOFIdx == s->getDOFXCtr()->getIndex() )
        {
            mediator->handleDOFColDeactivate();
            mediator->handleDOFOpaDeactivate();
            s->setModeEdtDOFXCtr();
        }
        else if ( DOFIdx == s->getDOFYCtr()->getIndex() )
        {
            mediator->handleDOFColDeactivate();
            mediator->handleDOFOpaDeactivate();
            s->setModeEdtDOFYCtr();
        }
        else if ( DOFIdx == s->getDOFHgt()->getIndex() )
        {
            mediator->handleDOFColDeactivate();
            mediator->handleDOFOpaDeactivate();
            s->setModeEdtDOFHgt();
        }
        else if ( DOFIdx == s->getDOFWth()->getIndex() )
        {
            mediator->handleDOFColDeactivate();
            mediator->handleDOFOpaDeactivate();
            s->setModeEdtDOFWth();
        }
        else if ( DOFIdx == s->getDOFAgl()->getIndex() )
        {
            mediator->handleDOFColDeactivate();
            mediator->handleDOFOpaDeactivate();
            s->setModeEdtDOFAgl();
        }
        else if ( DOFIdx == s->getDOFCol()->getIndex() )
        {
            s->setModeEdtDOFCol();
            mediator->handleDOFColActivate();
            mediator->handleDOFOpaDeactivate();
        }
        else if ( DOFIdx == s->getDOFOpa()->getIndex() )
        {   
            s->setModeEdtDOFOpa();
            mediator->handleDOFColDeactivate();
            mediator->handleDOFOpaActivate();
        }
        s = NULL;

        canvas->Refresh();
    }
}


// ----------------------------------------
void DiagramEditor::handleDOFSetTextStatus(
    const int &DOFIdx,
    const int &status )
// ----------------------------------------
{
    Shape* s = NULL;
    int sizeShapes = diagram->getSizeShapes();
    for ( int i = 0; i < sizeShapes; ++i )    
    {
        if ( diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_XCTR || 
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_YCTR ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_HGT  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_WTH  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_AGL  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_COL  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_OPA )
        {
            s = diagram->getShape( i );
            break;
        }
    }

    if ( s != NULL )
    {
        if ( DOFIdx == s->getDOFXCtr()->getIndex() )
            s->getDOFXCtr()->setTextStatus( status );
        else if ( DOFIdx == s->getDOFYCtr()->getIndex() )
            s->getDOFYCtr()->setTextStatus( status );
        else if ( DOFIdx == s->getDOFHgt()->getIndex() )
            s->getDOFHgt()->setTextStatus( status );
        else if ( DOFIdx == s->getDOFWth()->getIndex() )
            s->getDOFWth()->setTextStatus( status );
        else if ( DOFIdx == s->getDOFAgl()->getIndex() )
            s->getDOFAgl()->setTextStatus( status );
        else if ( DOFIdx == s->getDOFCol()->getIndex() )
            s->getDOFCol()->setTextStatus( status );
        else if ( DOFIdx == s->getDOFOpa()->getIndex() )
            s->getDOFOpa()->setTextStatus( status );
        s = NULL;
    }
}


// -----------------------------------------------------------
int DiagramEditor::handleDOFGetTextStatus( const int &DOFIdx )
// -----------------------------------------------------------
{
    int result = -1;

    Shape* s = NULL;
    int sizeShapes = diagram->getSizeShapes();
    for ( int i = 0; i < sizeShapes; ++i )    
    {
        if ( diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_XCTR || 
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_YCTR ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_HGT  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_WTH  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_AGL  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_COL  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_OPA )
        {
            s = diagram->getShape( i );
            break;
        }
    }

    if ( s != NULL )
    {
        if ( DOFIdx == s->getDOFXCtr()->getIndex() )
            result = s->getDOFXCtr()->getTextStatus();
        else if ( DOFIdx == s->getDOFYCtr()->getIndex() )
            result = s->getDOFYCtr()->getTextStatus();
        else if ( DOFIdx == s->getDOFHgt()->getIndex() )
            result = s->getDOFHgt()->getTextStatus();
        else if ( DOFIdx == s->getDOFWth()->getIndex() )
            result = s->getDOFWth()->getTextStatus();
        else if ( DOFIdx == s->getDOFAgl()->getIndex() )
            result = s->getDOFAgl()->getTextStatus();            
        else if ( DOFIdx == s->getDOFCol()->getIndex() )
            result = s->getDOFCol()->getTextStatus();            
        else if ( DOFIdx == s->getDOFOpa()->getIndex() )
            result = s->getDOFOpa()->getTextStatus();
    }

    return result;
}


// ---------------------------------
void DiagramEditor::handleDOFColAdd(
    const double &hue,
    const double &y )
// ---------------------------------
{
    Shape* s = NULL;

    int sizeShapes = diagram->getSizeShapes();
    for ( int i = 0; i < sizeShapes; ++i )    
    {
        if ( diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_COL )
        {
            s = diagram->getShape( i );
            break;
        }
    }

    if ( s != NULL )
    {
        s->getDOFCol()->addValue( hue );
        s->addDOFColYValue( y );
    }

    s = NULL;
}


// ------------------------------------
void DiagramEditor::handleDOFColUpdate(
    const int &idx,
    const double &hue,
    const double &y )
// ------------------------------------
{
    Shape* s = NULL;

    int sizeShapes = diagram->getSizeShapes();
    for ( int i = 0; i < sizeShapes; ++i )    
    {
        if ( diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_COL )
        {
            s = diagram->getShape( i );
            break;
        }
    }

    if ( s != NULL )
    {
        s->getDOFCol()->setValue( idx, hue );
        s->setDOFColYValue( idx, y );
    }

    s = NULL;
}


// -----------------------------------
void DiagramEditor::handleDOFColClear(
    const int &idx )
// -----------------------------------
{
    Shape* s = NULL;

    int sizeShapes = diagram->getSizeShapes();
    for ( int i = 0; i < sizeShapes; ++i )    
    {
        if ( diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_COL )
        {
            s = diagram->getShape( i );
            break;
        }
    }

    if ( s != NULL )
    {
        if ( s->getDOFCol()->getSizeValues() > 2 )
        {
            s->getDOFCol()->clearValue( idx );
            s->clearDOFColYValue( idx );

            vector< double > hues;
            vector< double > yVals;

            s->getDOFCol()->getValues( hues );
            s->getDOFColYValues( yVals );
            
            mediator->handleDOFColSetValuesEdt( hues, yVals );
        }
    }

    s = NULL;
}


// ---------------------------------
void DiagramEditor::handleDOFOpaAdd(
    const double &hue,
    const double &y )
// ---------------------------------
{
    Shape* s = NULL;

    int sizeShapes = diagram->getSizeShapes();
    for ( int i = 0; i < sizeShapes; ++i )    
    {
        if ( diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_OPA )
        {
            s = diagram->getShape( i );
            break;
        }
    }

    if ( s != NULL )
    {
        s->getDOFOpa()->addValue( hue );
        s->addDOFOpaYValue( y );
    }

    s = NULL;
}


// ------------------------------------
void DiagramEditor::handleDOFOpaUpdate(
    const int &idx,
    const double &opa,
    const double &y )
// ------------------------------------
{
    Shape* s = NULL;

    int sizeShapes = diagram->getSizeShapes();
    for ( int i = 0; i < sizeShapes; ++i )    
    {
        if ( diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_OPA )
        {
            s = diagram->getShape( i );
            break;
        }
    }

    if ( s != NULL )
    {
        s->getDOFOpa()->setValue( idx, opa );
        s->setDOFOpaYValue( idx, y );
    }

    s = NULL;
}


// -----------------------------------
void DiagramEditor::handleDOFOpaClear(
    const int &idx )
// -----------------------------------
{
    Shape* s = NULL;

    int sizeShapes = diagram->getSizeShapes();
    for ( int i = 0; i < sizeShapes; ++i )    
    {
        if ( diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_OPA )
        {
            s = diagram->getShape( i );
            break;
        }
    }

    if ( s != NULL )
    {
        if ( s->getDOFOpa()->getSizeValues() > 2 )
        {
            s->getDOFOpa()->clearValue( idx );
            s->clearDOFOpaYValue( idx );

            vector< double > opas;
            vector< double > yVals;

            s->getDOFOpa()->getValues( opas );
            s->getDOFOpaYValues( yVals );
            
            mediator->handleDOFOpaSetValuesEdt( opas, yVals );
        }
    }

    s = NULL;
}


// --------------------------------
void DiagramEditor::setLinkDOFAttr( 
    const int &DOFIdx, 
    const int &attrIdx )
// --------------------------------
{
    Shape* s = NULL;

    int sizeShapes = diagram->getSizeShapes();
    for ( int i = 0; i < sizeShapes; ++i )    
    {
        if ( diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_XCTR || 
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_YCTR ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_HGT  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_WTH  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_AGL  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_COL  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_OPA )
        {
            s = diagram->getShape( i );
            break;
        }
    }

    if ( s != NULL )
    {
        Attribute* a = graph->getAttribute( attrIdx );
        DOF* dof = NULL;

        if ( DOFIdx == s->getDOFXCtr()->getIndex() )
           dof = s->getDOFXCtr();
        else if ( DOFIdx == s->getDOFYCtr()->getIndex() )
           dof = s->getDOFYCtr();
        else if ( DOFIdx == s->getDOFWth()->getIndex() )
           dof = s->getDOFWth();
        else if ( DOFIdx == s->getDOFHgt()->getIndex() )
           dof = s->getDOFHgt();
        else if ( DOFIdx == s->getDOFAgl()->getIndex() )
           dof = s->getDOFAgl();
        else if ( DOFIdx == s->getDOFCol()->getIndex() )
           dof = s->getDOFCol();
        else if ( DOFIdx == s->getDOFOpa()->getIndex() )
           dof = s->getDOFOpa();

        if ( dof != NULL )
            dof->setAttribute( a );
        
        displDOFInfo( s );

        a   = NULL;
        dof = NULL;
    }

    s = NULL;
}


// ------------------------------------------------------
void DiagramEditor::clearLinkDOFAttr( const int &DOFIdx )
// ------------------------------------------------------
{
    Shape* s = NULL;

    int sizeShapes = diagram->getSizeShapes();
    for ( int i = 0; i < sizeShapes; ++i )    
    {
        if ( diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_XCTR || 
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_YCTR ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_HGT  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_WTH  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_AGL  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_COL  ||
             diagram->getShape( i )->getMode() == Shape::MODE_EDT_DOF_OPA )
        {
            s = diagram->getShape( i );
            break;
        }
    }

    if ( s != NULL )
    {
        DOF* dof = NULL;

        if ( DOFIdx == s->getDOFXCtr()->getIndex() )
           dof = s->getDOFXCtr();
        else if ( DOFIdx == s->getDOFYCtr()->getIndex() )
           dof = s->getDOFYCtr();
        else if ( DOFIdx == s->getDOFWth()->getIndex() )
           dof = s->getDOFWth();
        else if ( DOFIdx == s->getDOFHgt()->getIndex() )
           dof = s->getDOFHgt();
        else if ( DOFIdx == s->getDOFAgl()->getIndex() )
           dof = s->getDOFAgl();
        else if ( DOFIdx == s->getDOFCol()->getIndex() )
           dof = s->getDOFCol();
        else if ( DOFIdx == s->getDOFOpa()->getIndex() )
           dof = s->getDOFOpa();

        if ( dof != NULL )
            dof->setAttribute( NULL );

        displDOFInfo( s );

        dof = NULL;
    }

    s = NULL;
}


// -------------------------------------------------------
void DiagramEditor::clearLinkAttrDOF( const int &attrIdx )
// -------------------------------------------------------
{
    Shape* s        = NULL;
    DOF* dof        = NULL;
    Attribute* attr = NULL;

    int sizeShapes = diagram->getSizeShapes();
    for ( int i = 0; i < sizeShapes; ++i )    
    {
        s = diagram->getShape( i );
        
        dof  = s->getDOFXCtr();
        attr = dof->getAttribute();
        if ( attr != NULL && attr->getIndex() == attrIdx )
            dof->setAttribute( NULL );
        
        dof  = s->getDOFYCtr();
        attr = dof->getAttribute();
        if ( attr != NULL && attr->getIndex() == attrIdx )
            dof->setAttribute( NULL );

        dof  = s->getDOFWth();
        attr = dof->getAttribute();
        if ( attr != NULL && attr->getIndex() == attrIdx )
            dof->setAttribute( NULL );

        dof  = s->getDOFHgt();
        attr = dof->getAttribute();
        if ( attr != NULL && attr->getIndex() == attrIdx )
            dof->setAttribute( NULL );

        dof  = s->getDOFAgl();
        attr = dof->getAttribute();
        if ( attr != NULL && attr->getIndex() == attrIdx )
            dof->setAttribute( NULL );

        dof  = s->getDOFCol();
        attr = dof->getAttribute();
        if ( attr != NULL && attr->getIndex() == attrIdx )
            dof->setAttribute( NULL );

        dof  = s->getDOFOpa();
        attr = dof->getAttribute();
        if ( attr != NULL && attr->getIndex() == attrIdx )
            dof->setAttribute( NULL );
    }

    s    = NULL;
    dof  = NULL;
    attr = NULL;
}


// -- get functions ---------------------------------------------
    

// ---------------------------------
Diagram* DiagramEditor::getDiagram()
// ---------------------------------
{
    return diagram;
}


// -----------------------------
int DiagramEditor::getEditMode()
// -----------------------------
{
    return editMode;
}


// -- visualization functions  ----------------------------------


// ------------------------------------------------------
void DiagramEditor::visualize( const bool &inSelectMode )
// ------------------------------------------------------
{
    clear();

    if ( inSelectMode == true )
    {
        if ( editMode == EDIT_MODE_SELECT ||
             editMode == EDIT_MODE_DOF )
        {
            // set up picking
            GLint hits = 0;
            GLuint selectBuf[512];
            startSelectMode(
                hits,
                selectBuf,
                2.0,
                2.0 );

            // render in select mode
            glPushName( 0 );
            diagram->visualize( inSelectMode, canvas );
            glPopName();

            // finish up picking
            finishSelectMode(
                hits,
                selectBuf );
        }
    }
    else
    {
        diagram->visualize( inSelectMode, canvas );

        if ( mouseDrag == MSE_DRAG_TRUE &&
             mouseSide == MSE_SIDE_LFT )
        {
            double x1, y1;
            double x2, y2;
            double pix;

            canvas->getWorldCoords( xMouseDragBeg, yMouseDragBeg, x1, y1 );
            canvas->getWorldCoords( xMouseCur, yMouseCur, x2, y2 );
            
            if ( diagram->getSnapGrid() == true )
            {
                double intv = diagram->getGridInterval( canvas );
                
                x1 = Utils::rndToNearestMult( x1, intv );
                y1 = Utils::rndToNearestMult( y1, intv );
                x2 = Utils::rndToNearestMult( x2, intv );
                y2 = Utils::rndToNearestMult( y2, intv );
            }
            
            pix = canvas->getPixelSize();

            double dX, dY;
            double xC, yC;

            dX = x2-x1;
            dY = y2-y1;

            xC = x1+0.5*dX;
            yC = y1+0.5*dY;
    
            VisUtils::setColorDkGray();
            if ( editMode == EDIT_MODE_RECT )
                VisUtils::drawRect( x1, x2, y1, y2 );
            else if ( editMode == EDIT_MODE_ELLIPSE )
                VisUtils::drawEllipse( xC, yC, 0.5*dX, 0.5*dY, Shape::segNumHnt );
            else if ( editMode == EDIT_MODE_LINE )
                VisUtils::drawLine( x1, x2, y1, y2 );
            else if ( editMode == EDIT_MODE_ARROW )
                VisUtils::drawArrow( x1, x2, y1, y2, Shape::hdlSzeHnt*pix, 2.0*Shape::hdlSzeHnt*pix );
            else if ( editMode == EDIT_MODE_DARROW )
                VisUtils::drawDArrow( x1, x2, y1, y2, Shape::hdlSzeHnt*pix, 2.0*Shape::hdlSzeHnt*pix );
        }
    }
}


// -- event handlers --------------------------------------------


// -----------------------------------------
void DiagramEditor::handleMouseLftDownEvent(
    const int &x,
    const int &y )
// -----------------------------------------
{
    Visualizer::handleMouseLftDownEvent( x, y );
    
    if ( editMode == EDIT_MODE_SELECT )
        visualize( true );
    else if ( editMode == EDIT_MODE_DOF )
        visualize( true );
}
    

// ---------------------------------------
void DiagramEditor::handleMouseLftUpEvent( 
    const int &x, 
    const int &y )
// ---------------------------------------
{
    Visualizer::handleMouseLftUpEvent( x, y );

    if ( editMode == EDIT_MODE_SELECT )
        visualize( true );
    else if ( editMode == EDIT_MODE_DOF )
        visualize( true );
    else
    {
        double w, h;
        double x1, x2, y1, y2;
        double dX, dY;
        double pix;
        
        canvas->getSize( w, h );
        pix = canvas->getPixelSize();
        
        // do transl & scale here
        
        canvas->getWorldCoords( xMouseDragBeg, yMouseDragBeg, x1, y1 );
        canvas->getWorldCoords( xMouseCur,     yMouseCur,     x2, y2 );
        
        canvas->getWorldCoords( xMouseDragBeg, yMouseDragBeg, x1, y1 );
        canvas->getWorldCoords( xMouseCur, yMouseCur, x2, y2 );
            
        if ( diagram->getSnapGrid() == true )
        {
            double intv = diagram->getGridInterval( canvas );
                
            x1 = Utils::rndToNearestMult( x1, intv );
            y1 = Utils::rndToNearestMult( y1, intv );
            x2 = Utils::rndToNearestMult( x2, intv );
            y2 = Utils::rndToNearestMult( y2, intv );
        }

        dX = x2-x1;
        dY = y2-y1;
        
        if ( Utils::abs( dX ) < Shape::minSzeHnt*pix && 
             Utils::abs( dY ) < Shape::minSzeHnt*pix )
        {
            dX = Shape::minSzeHnt*pix;
            dY = Shape::minSzeHnt*pix;
        }
        
        double xC, yC;
        xC = x1+0.5*dX;
        yC = y1+0.5*dY;

        Shape* s = new Shape(
            mediator,
            diagram->getSizeShapes(), 
            xC,     yC,
            0.5*dX, -0.5*dY,
            0.0,    Shape::TYPE_RECT );
    
        if ( editMode == EDIT_MODE_RECT )
            s->setTypeRect();
        else if ( editMode == EDIT_MODE_ELLIPSE )
            s->setTypeEllipse();
        else if ( editMode == EDIT_MODE_LINE )
            s->setTypeLine();
        else if ( editMode == EDIT_MODE_ARROW )
            s->setTypeArrow();
        else if ( editMode == EDIT_MODE_DARROW )
            s->setTypeDArrow();

        diagram->addShape( s );
        s = NULL;
        
        // undo transl & scale here
    }
}


// -------------------------------------------
void DiagramEditor::handleMouseLftDClickEvent(
    const int &x,
    const int &y )
// -------------------------------------------
{
    Visualizer::handleMouseLftDClickEvent( x, y );
    
    if ( editMode == EDIT_MODE_SELECT )
        visualize( true );
}


// -----------------------------------------
void DiagramEditor::handleMouseRgtDownEvent( 
    const int &x, 
    const int &y )
// -----------------------------------------
{
    Visualizer::handleMouseRgtDownEvent( x, y );

    if ( editMode == EDIT_MODE_SELECT )
        visualize( true );
}


// ----------------------------------------
void DiagramEditor::handleMouseMotionEvent( 
    const int &x, 
    const int &y )
// ----------------------------------------
{
    Visualizer::handleMouseMotionEvent( x, y );

    if ( ( editMode  == EDIT_MODE_SELECT || editMode  == EDIT_MODE_DOF ) &&
          mouseDrag == MSE_DRAG_TRUE )
    {
        if ( drgBegIdx1 < 0 && drgBegIdx2 < 0 )
            // select mode
            visualize( true );
        else
            handleDrag();
    }

    xMousePrev = xMouseCur;
    yMousePrev = yMouseCur;
}


// -------------------------------------------------------
void DiagramEditor::handleKeyUpEvent( const int &keyCode )
// -------------------------------------------------------
{
    Visualizer::handleKeyUpEvent( keyCode );

    if ( editMode == EDIT_MODE_SELECT )
    {
        if ( keyCode == WXK_DELETE )
        {
            handleDelete();
        }
    }
}


// -------------------------------------------------------
void DiagramEditor::handleHits( const vector< int > &ids )
// -------------------------------------------------------
{
    /*
    *mediator << "Hit: ";
    for ( int i = 0; i < ids.size(); ++i )
    {
        *mediator << "[";
        *mediator << ids[i];
        *mediator << "] ";
    }
    *mediator << "\n";
    */

    // only diagram was hit
    if ( ids.size() == 1 )
        handleHitDiagramOnly();  
    // shape was hit
    else if ( ids.size() == 2 )
        handleHitShape( ids[1] );
    else if ( ids.size() == 3 )
        handleHitShapeHandle( ids[1], ids[2] );
}


// ---------------------------------------
void DiagramEditor::handleHitDiagramOnly()
// ---------------------------------------
{
    if ( drgBegIdx1 < 0 && drgBegIdx2 < 0 )
    {
        deselectAll();
        mediator->handleDOFDeselect();
    }
    else
    {
        drgBegIdx1 = -1;
        drgBegIdx2 = -1;
    }

    if ( mouseSide == MSE_SIDE_RGT )
    {
        canvas->getWorldCoords( 
            xMouseCur, yMouseCur, 
            xPaste,    yPaste );
            
        bool pasteFlag = false;
        if ( clipBoardShape != NULL )
            pasteFlag = true;
        mediator->handleEditShape(
            false,     // cut
            false,     // copy
            pasteFlag, // paste
            false,     // delete
            false,     // bring to front
            false,     // send to back
            false,     // bring forward
            false,     // send backward
            false );   // edit DOF
    }
}


// ------------------------------------------------------
void DiagramEditor::handleHitShape( const int &shapeIdx )
// ------------------------------------------------------
{
    int sizeShapes = 0;
    if ( diagram != NULL )
        sizeShapes = diagram->getSizeShapes();

    if ( 0 <= shapeIdx && shapeIdx < sizeShapes )
    {
        Shape* s = diagram->getShape( shapeIdx );

        if ( mouseClick == MSE_CLICK_SINGLE )
        {
            if ( mouseButton == MSE_BUTTON_DOWN )
            {
                if ( mouseSide == MSE_SIDE_LFT )
                {
                    if ( editMode == EDIT_MODE_SELECT )
                    {
                        if ( s->getMode() == Shape::MODE_EDIT )
                        {
                            if ( drgBegIdx1 < 0 && drgBegIdx2 < 0 )
                                s->setModeNormal();
                            else
                            {
                                s->setModeEdit();
                                drgBegIdx1 = -1;
                                drgBegIdx2 = -1;
                            }
                        }
                        else
                            s->setModeEdit();
                        
                        for ( int i = 0; i < sizeShapes; ++i )
                            if ( i != shapeIdx )
                                diagram->getShape(i)->setModeNormal();
                    } // mode
                    else if ( editMode == EDIT_MODE_DOF )
                    {
                        if ( s->getMode() != Shape::MODE_EDT_DOF_XCTR &&
                             s->getMode() != Shape::MODE_EDT_DOF_YCTR &&
                             s->getMode() != Shape::MODE_EDT_DOF_WTH  &&
                             s->getMode() != Shape::MODE_EDT_DOF_HGT  &&
                             s->getMode() != Shape::MODE_EDT_DOF_AGL )
                        {
                            s->setMode( Shape::MODE_EDT_DOF_XCTR );
                            drgBegIdx1 = -1;
                            drgBegIdx2 = -1;
                        }
                        displDOFInfo( s );
					

                        for ( int i = 0; i < sizeShapes; ++i )
                            if ( i != shapeIdx )
                                diagram->getShape(i)->setModeNormal();
                    } // mode
                } // side
                else if ( mouseSide == MSE_SIDE_RGT )
                {
                    canvas->getWorldCoords( 
                        xMouseCur, yMouseCur, 
                        xPaste,    yPaste );

                    for ( int i = 0; i < sizeShapes; ++i )
                        if ( i != s->getIndex() )
                            diagram->getShape(i)->setModeNormal();
                
                    s->setModeEdit();
                    displShapeEdtOptions( s );
                } // side
            } // button
        } // click
	    
		canvas->Refresh();
        s = NULL;
		
		//Clear mouse input 
		mouseClick  = -1;
		mouseButton = -1;
		mouseSide   = -1; 
    }
}


// --------------------------------------
void DiagramEditor::handleHitShapeHandle( 
    const int &shapeIdx,
    const int &handleId )
// --------------------------------------
{
    int sizeShapes = 0;
    if ( diagram != NULL )
        sizeShapes = diagram->getSizeShapes();

    if ( 0 <= shapeIdx && shapeIdx < sizeShapes )
    {
		Shape* s = diagram->getShape( shapeIdx );

        if ( mouseClick == MSE_CLICK_SINGLE )
        {
            if ( mouseButton == MSE_BUTTON_DOWN )
            {
                if ( mouseSide == MSE_SIDE_LFT )
                {
                    if ( mouseDrag == MSE_DRAG_TRUE )
                    {
                        drgBegIdx1 = shapeIdx;
                        drgBegIdx2 = handleId;
                        xDrgDist   = 0.0;
                        yDrgDist   = 0.0;
                    }
                    else if ( mouseDrag == MSE_DRAG_FALSE )
                    {
                        drgBegIdx1 = -1;
                        drgBegIdx2 = -1;
                    }
                }
                else if ( mouseSide == MSE_SIDE_RGT )
                {
                    canvas->getWorldCoords( 
                        xMouseCur, yMouseCur, 
                        xPaste,    yPaste );

                    for ( int i = 0; i < sizeShapes; ++i )
                        if ( i != s->getIndex() )
                            diagram->getShape(i)->setModeNormal();
                
                    //s->setModeEdit();
                    displShapeEdtOptions( s );
                }
            }
            else if ( mouseButton == MSE_BUTTON_UP )
            {
                drgBegIdx1 = -1;
                drgBegIdx2 = -1;

                s->handleHit( handleId );
            }
        }
        else if ( mouseClick == MSE_CLICK_DOUBLE )
        {
            if ( mouseButton == MSE_BUTTON_UP )
            {
                if ( mouseSide == MSE_SIDE_LFT )
                {
                    if ( s->getMode() != Shape::MODE_EDT_DOF_XCTR &&
                         s->getMode() != Shape::MODE_EDT_DOF_YCTR &&
                         s->getMode() != Shape::MODE_EDT_DOF_WTH  &&
                         s->getMode() != Shape::MODE_EDT_DOF_HGT  &&
                         s->getMode() != Shape::MODE_EDT_DOF_AGL )
                    {
                        s->setMode( Shape::MODE_EDT_DOF_XCTR );
                        drgBegIdx1 = -1;
                        drgBegIdx2 = -1;
                    }
                    displDOFInfo( s );
                    editMode = EDIT_MODE_DOF;
                    mediator->handleEditModeDOF( this );
                    
                    for ( int i = 0; i < sizeShapes; ++i )
                        if ( i != shapeIdx )
                            diagram->getShape(i)->setModeNormal();
                }
            }
        }
       
		//Clear mouse input 
		mouseClick  = -1;
		mouseButton = -1;
		mouseSide   = -1; 
        s = NULL;
    }
}


// -----------------------------
void DiagramEditor::handleDrag()
// -----------------------------
{
    int sizeShapes = 0;
    if ( diagram != NULL )
        sizeShapes = diagram->getSizeShapes();

    if ( 0 <= drgBegIdx1 && drgBegIdx1 < sizeShapes )
    {
        // do transl & scale here
        
        Shape* s = diagram->getShape( drgBegIdx1 );

        if ( s->getMode() == Shape::MODE_EDIT )
        {
            if ( drgBegIdx2 == Shape::ID_HDL_CTR )
                handleDragCtr( s );
            else if ( drgBegIdx2 == Shape::ID_HDL_TOP_LFT )
                handleDragTopLft( s );
            else if ( drgBegIdx2 == Shape::ID_HDL_LFT )
                handleDragLft( s );
            else if ( drgBegIdx2 == Shape::ID_HDL_BOT_LFT )
                handleDragBotLft( s );
            else if ( drgBegIdx2 == Shape::ID_HDL_BOT )
                handleDragBot( s );
            else if ( drgBegIdx2 == Shape::ID_HDL_BOT_RGT )
                handleDragBotRgt( s );
            else if ( drgBegIdx2 == Shape::ID_HDL_RGT )
                handleDragRgt( s );
            else if ( drgBegIdx2 == Shape::ID_HDL_TOP_RGT )
                handleDragTopRgt( s );
            else if ( drgBegIdx2 == Shape::ID_HDL_TOP )
                handleDragTop( s );
            else if ( drgBegIdx2 == Shape::ID_HDL_ROT_RGT )
                handleDragRotRgt( s );
            else if ( drgBegIdx2 == Shape::ID_HDL_ROT_TOP )
                handleDragRotTop( s );
        }
        else if ( s->getMode() == Shape::MODE_EDT_DOF_XCTR )
        {
            /*
            if ( drgBegIdx2 == Shape::ID_HDL_DOF_BEG )
                handleDragDOFXCtrBeg( s );
            else
            */ 
            if ( drgBegIdx2 == Shape::ID_HDL_DOF_END )
                handleDragDOFXCtrEnd( s );
        }
        else if ( s->getMode() == Shape::MODE_EDT_DOF_YCTR )
        {
            /*
            if ( drgBegIdx2 == Shape::ID_HDL_DOF_BEG )
                handleDragDOFYCtrBeg( s );
            else 
            */
            if ( drgBegIdx2 == Shape::ID_HDL_DOF_END )
                handleDragDOFYCtrEnd( s );
        }
        else if ( s->getMode() == Shape::MODE_EDT_DOF_WTH )
        {
            /*
            if ( drgBegIdx2 == Shape::ID_HDL_DOF_BEG )
                handleDragDOFWthBeg( s );
            else
            */
            if ( drgBegIdx2 == Shape::ID_HDL_DOF_END )
                handleDragDOFWthEnd( s );
        }
        else if ( s->getMode() == Shape::MODE_EDT_DOF_HGT )
        {
            /*
            if ( drgBegIdx2 == Shape::ID_HDL_DOF_BEG )
                handleDragDOFHgtBeg( s );
            else
            */
            if ( drgBegIdx2 == Shape::ID_HDL_DOF_END )
                handleDragDOFHgtEnd( s );
        }
        else if ( s->getMode() == Shape::MODE_EDT_DOF_AGL )
        {
            if ( drgBegIdx2 == Shape::ID_HDL_HGE )            
                handleDragHge( s );
            /*
            else if ( drgBegIdx2 == Shape::ID_HDL_DOF_BEG )
                handleDragDOFAglBeg( s );
            */
            else if ( drgBegIdx2 == Shape::ID_HDL_DOF_END )
                handleDragDOFAglEnd( s );
        }
        
        // undo transl & scale here

        canvas->Refresh();
        s = NULL;
    }
}


// ----------------------------
void DiagramEditor::handleCut()
// ----------------------------
{
    Shape* origShape = NULL;
    Shape* copyShape = NULL;

    // find & copy selected shape
    for ( int i = 0; i < diagram->getSizeShapes() && copyShape == NULL; ++i )
    {
        if ( diagram->getShape(i)->getMode() != Shape::MODE_NORMAL )
        {
            origShape = diagram->getShape( i );
            
            // invoke copy constructor
            copyShape = new Shape( *origShape );
        }
    }

    // instantiate clipboard shape
    if ( copyShape != NULL )
    {
        // delete original shape
        diagram->deleteShape( origShape->getIndex() );

        // delete previous clipBoardShape
        if ( clipBoardShape != NULL )
        {
            delete clipBoardShape;
            clipBoardShape = NULL;
        }

        // update clipboard shape
        clipBoardShape = copyShape;
    }

    origShape = NULL;
    copyShape = NULL;

}


// -----------------------------
void DiagramEditor::handleCopy()
// -----------------------------
{
    Shape* origShape = NULL;
    Shape* copyShape = NULL;

    // find & copy selected shape
    for ( int i = 0; i < diagram->getSizeShapes() && copyShape == NULL; ++i )
    {
        if ( diagram->getShape(i)->getMode() != Shape::MODE_NORMAL )
        {
            origShape = diagram->getShape( i );
            
            // invoke copy constructor
            copyShape = new Shape( *origShape );
        }
    }

    // instantiate clipboard shape
    if ( copyShape != NULL )
    {
        // delete previous clipBoardShape
        if ( clipBoardShape != NULL )
        {
            delete clipBoardShape;
            clipBoardShape = NULL;
        }

        // update clipboard shape
        clipBoardShape = copyShape;
    }

    origShape = NULL;
    copyShape = NULL;
}


// ------------------------------
void DiagramEditor::handlePaste()
// ------------------------------
{
    if ( clipBoardShape != NULL )
    {
        double xC, yC;

        // update index of clipboard shape
        clipBoardShape->setIndex( diagram->getSizeShapes() );

        // update clipboard shape
        clipBoardShape->getCenter( xC, yC );
        clipBoardShape->setCenter( xPaste, yPaste );
        clipBoardShape->setModeEdit();
       
        // deselect all other shapes
        for ( int i = 0; i < diagram->getSizeShapes(); ++i )
            if ( diagram->getShape(i)->getMode() != Shape::MODE_NORMAL )
                diagram->getShape(i)->setModeNormal();
        
        // add clipboard shape to diagram
        diagram->addShape( clipBoardShape );

        // make another copy of clipboard shape
        clipBoardShape = new Shape( *clipBoardShape );
    }
}


// -------------------------------
void DiagramEditor::handleDelete()
// -------------------------------
{
    vector< int > toDelete;
    // get indices to delete
    {
    for ( int i = 0; i < diagram->getSizeShapes() ; ++i )
    {
        /*
        if ( diagram->getShape(i)->getMode() == Shape::MODE_EDT_CTR_DFC ||
             diagram->getShape(i)->getMode() == Shape::MODE_EDT_HGE_AGL )
        */
        if ( diagram->getShape(i)->getMode() == Shape::MODE_EDIT )
            toDelete.push_back( i );
    }
    }
    
    // delete shapes
    {
    for ( size_t i = 0; i < toDelete.size(); ++i )
        diagram->deleteShape( toDelete[i]-i );
    }
}


// -------------------------------------
void DiagramEditor::handleBringToFront()
// -------------------------------------
{
    Shape* s = NULL;
    for ( int i = 0; i < diagram->getSizeShapes() && s == NULL; ++i )
        if ( diagram->getShape(i)->getMode() != Shape::MODE_NORMAL )
            s = diagram->getShape(i);

    if ( s != NULL )
        diagram->moveShapeToBack( s->getIndex() );

    s = NULL;
}


// -----------------------------------
void DiagramEditor::handleSendToBack()
// -----------------------------------
{
    Shape* s = NULL;
    for ( int i = 0; i < diagram->getSizeShapes() && s == NULL; ++i )
        if ( diagram->getShape(i)->getMode() != Shape::MODE_NORMAL )
            s = diagram->getShape(i);

    if ( s != NULL )
        diagram->moveShapeToFront( s->getIndex() );

    s = NULL;
}


// -------------------------------------
void DiagramEditor::handleBringForward()
// -------------------------------------
{
    Shape* s = NULL;
    for ( int i = 0; i < diagram->getSizeShapes() && s == NULL; ++i )
        if ( diagram->getShape(i)->getMode() != Shape::MODE_NORMAL )
            s = diagram->getShape(i);

    if ( s != NULL )
        diagram->moveShapeBackward( s->getIndex() );

    s = NULL;
}


// -------------------------------------
void DiagramEditor::handleSendBackward()
// -------------------------------------
{
    Shape* s = NULL;
    for ( int i = 0; i < diagram->getSizeShapes() && s == NULL; ++i )
        if ( diagram->getShape(i)->getMode() != Shape::MODE_NORMAL )
            s = diagram->getShape(i);

    if ( s != NULL )
        diagram->moveShapeForward( s->getIndex() );

    s = NULL;
}


// --------------------------------
void DiagramEditor::handleEditDOF()
// --------------------------------
{
    Shape* s = NULL;
    for ( int i = 0; i < diagram->getSizeShapes() && s == NULL; ++i )
        if ( diagram->getShape(i)->getMode() != Shape::MODE_NORMAL )
            s = diagram->getShape(i);

    if ( s != NULL )
    {
        s->setModeEdtDOFXCtr();

        displDOFInfo( s );
        editMode = EDIT_MODE_DOF;
        mediator->handleEditModeDOF( this );
                    
        for ( int i = 0; i < diagram->getSizeShapes(); ++i )
            if ( i != s->getIndex() )
                diagram->getShape(i)->setModeNormal();
    
        s = NULL;
    }
}
    

// -- public utility functions --------------------------------------


// ------------------------------
void DiagramEditor::deselectAll()
// ------------------------------
{
    int sizeShapes = 0;
    if ( diagram != NULL )
        sizeShapes = diagram->getSizeShapes();

    for ( int i = 0; i < sizeShapes; ++i )
        diagram->getShape(i)->setModeNormal();
    
    drgBegIdx1 = -1;
    drgBegIdx2 = -1;
}


// -- private utility functions -------------------------------------


// -------------------------------------------------
void DiagramEditor::displShapeEdtOptions( Shape *s )
// -------------------------------------------------
{
    if ( s != NULL )
    {
        bool pasteFlag = false;
        if ( clipBoardShape != NULL )
            pasteFlag = true;
        
        mediator->handleEditShape(
            true,      // cut
            true,      // copy
            pasteFlag, // paste
            true,      // delete
            true,      // bring to front
            true,      // send to back
            true,      // bring forward
            true,      // send backward
            true );    // edit DOF
	}
}


// -----------------------------------------
void DiagramEditor::displDOFInfo( Shape* s )
// -----------------------------------------
{
    if ( s != NULL )
    {
        vector< int > indcs;
        vector< string > dofs;
        vector< int > attrIdcs;
        int selIdx = -1;

        indcs.push_back( s->getDOFXCtr()->getIndex() );
        dofs.push_back(  s->getDOFXCtr()->getLabel() );
        if ( s->getDOFXCtr()->getAttribute() == NULL )
            attrIdcs.push_back( -1 );
        else
            attrIdcs.push_back( s->getDOFXCtr()->getAttribute()->getIndex() );
        if ( s->getMode() == Shape::MODE_EDT_DOF_XCTR )
            selIdx = s->getDOFXCtr()->getIndex();

        indcs.push_back( s->getDOFYCtr()->getIndex() );
        dofs.push_back(  s->getDOFYCtr()->getLabel() );
        if ( s->getDOFYCtr()->getAttribute() == NULL )
            attrIdcs.push_back( -1 );
        else
            attrIdcs.push_back( s->getDOFYCtr()->getAttribute()->getIndex() );
        if ( s->getMode() == Shape::MODE_EDT_DOF_YCTR )
            selIdx = s->getDOFYCtr()->getIndex();

        indcs.push_back( s->getDOFWth()->getIndex() );
        dofs.push_back(  s->getDOFWth()->getLabel() );
        if ( s->getDOFWth()->getAttribute() == NULL )
            attrIdcs.push_back( -1 );
        else
            attrIdcs.push_back( s->getDOFWth()->getAttribute()->getIndex() );
        if ( s->getMode() == Shape::MODE_EDT_DOF_WTH )
            selIdx = s->getDOFWth()->getIndex();

        indcs.push_back( s->getDOFHgt()->getIndex() );
        dofs.push_back(  s->getDOFHgt()->getLabel() );
        if ( s->getDOFHgt()->getAttribute() == NULL )
            attrIdcs.push_back( -1 );
        else
            attrIdcs.push_back( s->getDOFHgt()->getAttribute()->getIndex() );
        if ( s->getMode() == Shape::MODE_EDT_DOF_HGT )
            selIdx = s->getDOFHgt()->getIndex();

        indcs.push_back( s->getDOFAgl()->getIndex() );
        dofs.push_back(  s->getDOFAgl()->getLabel() );
        if ( s->getDOFAgl()->getAttribute() == NULL )
            attrIdcs.push_back( -1 );
        else
            attrIdcs.push_back( s->getDOFAgl()->getAttribute()->getIndex() );
        if ( s->getMode() == Shape::MODE_EDT_DOF_AGL )
            selIdx = s->getDOFAgl()->getIndex();

        indcs.push_back( s->getDOFCol()->getIndex() );
        dofs.push_back(  s->getDOFCol()->getLabel() );
        if ( s->getDOFCol()->getAttribute() == NULL )
            attrIdcs.push_back( -1 );
        else
            attrIdcs.push_back( s->getDOFCol()->getAttribute()->getIndex() );
        if ( s->getMode() == Shape::MODE_EDT_DOF_COL )
            selIdx = s->getDOFCol()->getIndex();

        indcs.push_back( s->getDOFOpa()->getIndex() );
        dofs.push_back(  s->getDOFOpa()->getLabel() );
        if ( s->getDOFOpa()->getAttribute() == NULL )
            attrIdcs.push_back( -1 );
        else
            attrIdcs.push_back( s->getDOFOpa()->getAttribute()->getIndex() );
        if ( s->getMode() == Shape::MODE_EDT_DOF_OPA )
            selIdx = s->getDOFOpa()->getIndex();

        mediator->handleEditDOF(
            indcs,
            dofs,
            attrIdcs,
            selIdx );

        vector< double > vals;
        vector< double > yVals;

        s->getDOFCol()->getValues( vals );
        s->getDOFColYValues( yVals );
        mediator->handleDOFColSetValuesEdt( vals, yVals );

        vals.clear();
        s->getDOFOpa()->getValues( vals );
        s->getDOFOpaYValues( yVals );
        mediator->handleDOFOpaSetValuesEdt( vals, yVals );
    }
}


// ------------------------------------------
void DiagramEditor::handleDragCtr( Shape* s ) 
// ------------------------------------------
{
    double xPrv, yPrv;
    double xCur, yCur;
    double xCtr, yCtr;
    double x,    y;
    
    canvas->getWorldCoords( xMousePrev, yMousePrev, xPrv, yPrv );
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );
    s->getCenter( xCtr, yCtr );

    x = xCtr;
    y = yCtr;

    xDrgDist += xCur-xPrv;
    yDrgDist += yCur-yPrv;

    if ( diagram->getSnapGrid() == true )
    {
        x = Utils::rndToNearestMult( x+xDrgDist, diagram->getGridInterval( canvas ) );
        y = Utils::rndToNearestMult( y+yDrgDist, diagram->getGridInterval( canvas ) );

        if ( x != xCtr )
            xDrgDist = xCur-x;
        if ( y != yCtr )
            yDrgDist = yCur-y;
    }
    else
    {
        x += xCur-xPrv;
        y += yCur-yPrv;
    }

    s->setCenter( x, y );
}


// ---------------------------------------------
void DiagramEditor::handleDragTopLft( Shape* s )
// ---------------------------------------------
{
    double angl;             // shape's rotation angle in radians
    double xCtr, yCtr;       // center of shape
    double xDFC, yDFC;       // deltas of shape
    double xCur, yCur;       // current mouse position
    double x0,   y0;         // position after translating to shape's center
    double xS,   yS;         // position after rotating to shape's angle
    double hypX, adjX, oppX; // hypotenuse, adjacent & opposite sides
                             // for movement parallel to shape's x-axis
    double hypY, adjY, oppY; // hypotenuse, adjacent & opposite sides
                             // for movement parallel to shape's y-axis

    // get shape's geometry
    angl = Utils::degrToRad( s->getAngleCtr() );
    s->getCenter( xCtr, yCtr );
    s->getDFC( xDFC, yDFC );

    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );
    
    if ( diagram->getSnapGrid() == true )
    {
        double itv = diagram->getGridInterval( canvas );
        xCur = Utils::rndToNearestMult( xCur, itv );
        yCur = Utils::rndToNearestMult( yCur, itv );
    }

    // translate to xCtr, the 'origin'
    x0 = xCur-xCtr;
    y0 = yCur-yCtr;
    
    // rotate to 'normal' orientation, find x & y
    xS = x0*cos( -angl ) - y0*sin( -angl );
    yS = x0*sin( -angl ) + y0*cos( -angl );

    hypX = 0.5*(-xDFC-xS);
    adjX = hypX*cos( angl ); // x (-)
    oppX = hypX*sin( angl ); // y (-)

    hypY = 0.5*(yS-yDFC);  
    adjY = hypY*cos( angl ); // y (+)
    oppY = hypY*sin( angl ); // x (-)

    s->setCenter( xCtr-adjX-oppY, yCtr-oppX+adjY );
    s->setDFC(    xDFC+hypX,      yDFC+hypY );
}


// ------------------------------------------
void DiagramEditor::handleDragLft( Shape* s )
// ------------------------------------------
{
    double angl;          // shape's rotation angle in radians
    double xCtr, yCtr;    // center of shape
    double xDFC, yDFC;    // deltas of shape
    double xCur, yCur;    // current mouse position
    double x0,   y0;      // position after translating to shape's center
    double xS,   yS;      // position after rotating to shape's angle
    double hyp, adj, opp; // hypotenuse, adjacent & opposite sides
                          // for movement parallel to shape's x-axis

    // get shape's geometry
    angl = Utils::degrToRad( s->getAngleCtr() );
    s->getCenter( xCtr, yCtr );
    s->getDFC( xDFC, yDFC );

    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );
    
    // translate to xCtr, the 'origin'
    x0 = xCur-xCtr;
    y0 = yCur-yCtr;
    // rotate to 'normal' orientation, find x & y
    xS = x0*cos( -angl ) - y0*sin( -angl );
    yS = x0*sin( -angl ) + y0*cos( -angl );

    if ( diagram->getSnapGrid() == true )
    {
        double itv  = diagram->getGridInterval( canvas );
        double a = Utils::rndToNearestMult( xCtr+xS*cos(angl), itv )-xCtr;
        xS = a/cos(angl);
    }

    hyp = 0.5*(-xDFC-xS);
    adj = hyp*cos( angl ); // x (-)
    opp = hyp*sin( angl ); // y (-)

    s->setCenter( xCtr-adj, yCtr-opp );
    s->setDFC(    xDFC+hyp, yDFC );
}


// ---------------------------------------------
void DiagramEditor::handleDragBotLft( Shape* s )
// ---------------------------------------------
{
    double angl;             // shape's rotation angle in radians
    double xCtr, yCtr;       // center of shape
    double xDFC, yDFC;       // deltas of shape
    double xCur, yCur;       // current mouse position
    double x0,   y0;         // position after translating to shape's center
    double xS,   yS;         // position after rotating to shape's angle
    double hypX, adjX, oppX; // hypotenuse, adjacent & opposite sides
                             // for movement parallel to shape's x-axis
    double hypY, adjY, oppY; // hypotenuse, adjacent & opposite sides
                             // for movement parallel to shape's y-axis

    // get shape's geometry
    angl = Utils::degrToRad( s->getAngleCtr() );
    s->getCenter( xCtr, yCtr );
    s->getDFC( xDFC, yDFC );

    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );
    
    if ( diagram->getSnapGrid() == true )
    {
        double itv = diagram->getGridInterval( canvas );
        xCur = Utils::rndToNearestMult( xCur, itv );
        yCur = Utils::rndToNearestMult( yCur, itv );
    }

    // translate to xCtr, the 'origin'
    x0 = xCur-xCtr;
    y0 = yCur-yCtr;
    // rotate to 'normal' orientation, find x & y
    xS = x0*cos( -angl ) - y0*sin( -angl );
    yS = x0*sin( -angl ) + y0*cos( -angl );

    hypX = 0.5*(-xDFC-xS);
    adjX = hypX*cos( angl ); // x (-)
    oppX = hypX*sin( angl ); // y (-)

    hypY = 0.5*(-yDFC-yS);  
    adjY = hypY*cos( angl ); // y (-)
    oppY = hypY*sin( angl ); // x (+)

    s->setCenter( xCtr-adjX+oppY, yCtr-oppX-adjY );
    s->setDFC(    xDFC+hypX,      yDFC+hypY );
}


// ------------------------------------------
void DiagramEditor::handleDragBot( Shape* s )
// ------------------------------------------
{
    double angl;          // shape's rotation angle in radians
    double xCtr, yCtr;    // center of shape
    double xDFC, yDFC;    // deltas of shape
    double xCur, yCur;    // current mouse position
    double x0,   y0;      // position after translating to shape's center
    double xS,   yS;      // position after rotating to shape's angle
    double hyp, adj, opp; // hypotenuse, adjacent & opposite sides
                          // for movement parallel to shape's y-axis

    // get shape's geometry
    angl = Utils::degrToRad( s->getAngleCtr() );
    s->getCenter( xCtr, yCtr );
    s->getDFC( xDFC, yDFC );

    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );
    
    // translate to xCtr, the 'origin'
    x0 = xCur-xCtr;
    y0 = yCur-yCtr;
    // rotate to 'normal' orientation, find x & y
    xS = x0*cos( -angl ) - y0*sin( -angl );
    yS = x0*sin( -angl ) + y0*cos( -angl );

    if ( diagram->getSnapGrid() == true )
    {
        double itv  = diagram->getGridInterval( canvas );
        double a = Utils::rndToNearestMult( yCtr+yS*cos(angl), itv )-yCtr;
        yS = a/cos(angl);
    }

    hyp = 0.5*(-yDFC-yS);  
    adj = hyp*cos( angl ); // y (-)
    opp = hyp*sin( angl ); // x (+)

    s->setCenter( xCtr+opp, yCtr-adj );
    s->setDFC(    xDFC,     yDFC+hyp );
}


// ---------------------------------------------
void DiagramEditor::handleDragBotRgt( Shape* s )
// ---------------------------------------------
{
    double angl;             // shape's rotation angle in radians
    double xCtr, yCtr;       // center of shape
    double xDFC, yDFC;       // deltas of shape
    double xCur, yCur;       // current mouse position
    double x0,   y0;         // position after translating to shape's center
    double xS,   yS;         // position after rotating to shape's angle
    double hypX, adjX, oppX; // hypotenuse, adjacent & opposite sides
                             // for movement parallel to shape's x-axis
    double hypY, adjY, oppY; // hypotenuse, adjacent & opposite sides
                             // for movement parallel to shape's y-axis

    // get shape's geometry
    angl = Utils::degrToRad( s->getAngleCtr() );
    s->getCenter( xCtr, yCtr );
    s->getDFC( xDFC, yDFC );

    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );

    if ( diagram->getSnapGrid() == true )
    {
        double itv = diagram->getGridInterval( canvas );
        xCur = Utils::rndToNearestMult( xCur, itv );
        yCur = Utils::rndToNearestMult( yCur, itv );
    }
    
    // translate to xCtr, the 'origin'
    x0 = xCur-xCtr;
    y0 = yCur-yCtr;
    // rotate to 'normal' orientation, find x & y
    xS = x0*cos( -angl ) - y0*sin( -angl );
    yS = x0*sin( -angl ) + y0*cos( -angl );

    hypX = 0.5*(xS-xDFC);
    adjX = hypX*cos( angl ); // x (+)
    oppX = hypX*sin( angl ); // y (+)

    hypY = 0.5*(-yDFC-yS);  
    adjY = hypY*cos( angl ); // y (-)
    oppY = hypY*sin( angl ); // x (+)

    s->setCenter( xCtr+adjX+oppY, yCtr+oppX-adjY );
    s->setDFC(    xDFC+hypX,      yDFC+hypY );
}


// ------------------------------------------
void DiagramEditor::handleDragRgt( Shape* s )
// ------------------------------------------
{
    double angl;          // shape's rotation angle in radians
    double xCtr, yCtr;    // center of shape
    double xDFC, yDFC;    // deltas of shape
    double xCur, yCur;    // current mouse position
    double x0,   y0;      // position after translating to shape's center
    double xS,   yS;      // position after rotating to shape's angle
    double hyp, adj, opp; // hypotenuse, adjacent & opposite sides
                          // for movement parallel to shape's x-axis

    // get shape's geometry
    angl = Utils::degrToRad( s->getAngleCtr() );
    s->getCenter( xCtr, yCtr );
    s->getDFC( xDFC, yDFC );

    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );

    // translate to xCtr, the 'origin'
    x0 = xCur-xCtr;
    y0 = yCur-yCtr;

    // rotate to 'normal' orientation, find x & y
    xS = x0*cos( -angl ) - y0*sin( -angl );
    yS = x0*sin( -angl ) + y0*cos( -angl );

    if ( diagram->getSnapGrid() == true )
    {
        double itv  = diagram->getGridInterval( canvas );
        double a = Utils::rndToNearestMult( xCtr+xS*cos(angl), itv )-xCtr;
        xS = a/cos(angl);
    }

    hyp = 0.5*(xS-xDFC);
    adj = hyp*cos( angl ); // x (+)
    opp = hyp*sin( angl ); // y (+)

    s->setCenter( xCtr+adj, yCtr+opp );
    s->setDFC(    xDFC+hyp, yDFC );
}


// ---------------------------------------------
void DiagramEditor::handleDragTopRgt( Shape* s )
// ---------------------------------------------
{
    double angl;             // shape's rotation angle in radians
    double xCtr, yCtr;       // center of shape
    double xDFC, yDFC;       // deltas of shape
    double xCur, yCur;       // current mouse position
    double x0,   y0;         // position after translating to shape's center
    double xS,   yS;         // position after rotating to shape's angle
    double hypX, adjX, oppX; // hypotenuse, adjacent & opposite sides
                             // for movement parallel to shape's x-axis
    double hypY, adjY, oppY; // hypotenuse, adjacent & opposite sides
                             // for movement parallel to shape's y-axis

    // get shape's geometry
    angl = Utils::degrToRad( s->getAngleCtr() );
    s->getCenter( xCtr, yCtr );
    s->getDFC( xDFC, yDFC );

    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );

    if ( diagram->getSnapGrid() == true )
    {
        double itv = diagram->getGridInterval( canvas );
        xCur = Utils::rndToNearestMult( xCur, itv );
        yCur = Utils::rndToNearestMult( yCur, itv );
    }
    
    // translate to xCtr, the 'origin'
    x0 = xCur-xCtr;
    y0 = yCur-yCtr;
    // rotate to 'normal' orientation, find x & y
    xS = x0*cos( -angl ) - y0*sin( -angl );
    yS = x0*sin( -angl ) + y0*cos( -angl );

    hypX = 0.5*(xS-xDFC);
    adjX = hypX*cos( angl ); // x (+)
    oppX = hypX*sin( angl ); // y (+)

    hypY = 0.5*(yS-yDFC);  
    adjY = hypY*cos( angl ); // y (+)
    oppY = hypY*sin( angl ); // x (-)

    s->setCenter( xCtr+adjX-oppY, yCtr+oppX+adjY );
    s->setDFC(    xDFC+hypX,      yDFC+hypY );
}


// ------------------------------------------
void DiagramEditor::handleDragTop( Shape* s )
// ------------------------------------------
{
    double angl;          // shape's rotation angle in radians
    double xCtr, yCtr;    // center of shape
    double xDFC, yDFC;    // deltas of shape
    double xCur, yCur;    // current mouse position
    double x0,   y0;      // position after translating to shape's center
    double xS,   yS;      // position after rotating to shape's angle
    double hyp, adj, opp; // hypotenuse, adjacent & opposite sides
                          // for movement parallel to shape's y-axis

    // get shape's geometry
    angl = Utils::degrToRad( s->getAngleCtr() );
    s->getCenter( xCtr, yCtr );
    s->getDFC( xDFC, yDFC );

    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );
    
    // translate to xCtr, the 'origin'
    x0 = xCur-xCtr;
    y0 = yCur-yCtr;
    // rotate to 'normal' orientation, find x & y
    xS = x0*cos( -angl ) - y0*sin( -angl );
    yS = x0*sin( -angl ) + y0*cos( -angl );

    if ( diagram->getSnapGrid() == true )
    {
        double itv  = diagram->getGridInterval( canvas );
        double a = Utils::rndToNearestMult( yCtr+yS*cos(angl), itv )-yCtr;
        yS = a/cos(angl);
    }

    hyp = 0.5*(yS-yDFC);  
    adj = hyp*cos( angl ); // y (+)
    opp = hyp*sin( angl ); // x (-)

    s->setCenter( xCtr-opp, yCtr+adj );
    s->setDFC(    xDFC,     yDFC+hyp );
}


// ---------------------------------------------
void DiagramEditor::handleDragRotRgt( Shape* s )
// ---------------------------------------------
{
    double aglRd;      // shape's rotation angle in radians
    double aglDg;      // shape's rotation angle in degrees
    double xCtr, yCtr; // center of shape
    double xDFC, yDFC; // deltas of shape
    double xCur, yCur; // current mouse position
    double x0,   y0;   // position after translating to shape's center
    double xS,   yS;   // position after rotating to shape's angle
    
    // get shape's geometry
    aglDg = s->getAngleCtr();
    
    aglRd = Utils::degrToRad( aglDg );
    s->getCenter( xCtr, yCtr );
    s->getDFC( xDFC, yDFC );

    // get mouse info
    canvas->getWorldCoords( xMouseCur, yMouseCur, xCur, yCur );

    // translate to xCtr, the 'origin'
    x0 = xCur-xCtr;
    y0 = yCur-yCtr;
    // rotate to 'normal' orientation, find x & y
    xS = x0*cos( -aglRd ) - y0*sin( -aglRd );
    yS = x0*sin( -aglRd ) + y0*cos( -aglRd );

    aglDg = aglDg + Utils::calcAngleDg( xS, yS );

    if ( diagram->getSnapGrid() == true )
    {
        double itv = diagram->getAngleInterval();
        aglDg = Utils::rndToNearestMult( aglDg, itv );
    }
    
    if ( xDFC < 0 )
        // shape reflected about y-axis
        aglDg += 180.0;
    if ( aglDg >= 360.0 )
        aglDg -= 360.0;
    
    // update angle
    s->setAngleCtr( aglDg );
}


// ---------------------------------------------
void DiagramEditor::handleDragRotTop( Shape* s )
// ---------------------------------------------
{
    double aglRd;      // shape's rotation angle in radians
    double aglDg;      // shape's rotation angle in degrees
    double xCtr, yCtr; // center of shape
    double xDFC, yDFC; // deltas of shape
    double xCur, yCur; // current mouse position
    double x0,   y0;   // position after translating to shape's center
    double xS,   yS;   // position after rotating to shape's angle
    
    // get shape's geometry
    aglDg = s->getAngleCtr();
    aglRd = Utils::degrToRad( aglDg );
    s->getCenter( xCtr, yCtr );
    s->getDFC( xDFC, yDFC );

    // get mouse info
    canvas->getWorldCoords( xMouseCur, yMouseCur, xCur, yCur );
    
    // translate to xCtr, the 'origin'
    x0 = xCur-xCtr;
    y0 = yCur-yCtr;
    // rotate to 'normal' orientation, find x & y
    xS = x0*cos( -aglRd ) - y0*sin( -aglRd );
    yS = x0*sin( -aglRd ) + y0*cos( -aglRd );

    aglDg = aglDg + Utils::calcAngleDg( xS, yS ) - 90.0;

    if ( diagram->getSnapGrid() == true )
    {
        double itv = diagram->getAngleInterval();
        aglDg = Utils::rndToNearestMult( aglDg, itv );
    }
    
    if ( yDFC < 0 )
        // shape reflected about y-axis
        aglDg += 180.0;
    if ( aglDg >= 360.0 )
        aglDg -= 360.0;

    // update angle
    s->setAngleCtr( aglDg );
}


// -------------------------------------------------
void DiagramEditor::handleDragDOFXCtrBeg( Shape* s )
// -------------------------------------------------
{    
    double xCtr, yCtr;    // center of shape
    double xCur, yCur;    // current mouse position
    double x0;            // x position after translating to shape's center

    // get shape's geometry
    s->getCenter( xCtr, yCtr );
    
    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );

    // translate to center, the 'origin'
    x0 = xCur-xCtr;

    if ( diagram->getSnapGrid() == true )
    {
        double itv  = diagram->getGridInterval( canvas );
        x0 = Utils::rndToNearestMult( xCtr+x0, itv)-xCtr;
    }

    s->getDOFXCtr()->setMin( x0 );
}


// -------------------------------------------------
void DiagramEditor::handleDragDOFXCtrEnd( Shape* s )
// -------------------------------------------------
{
    double xCtr, yCtr;    // center of shape
    double xCur, yCur;    // current mouse position
    double x0;            // x position after translating to shape's center

    // get shape's geometry
    s->getCenter( xCtr, yCtr );
    
    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );

    // translate to center, the 'origin'
    x0 = xCur-xCtr;

    if ( diagram->getSnapGrid() == true )
    {
        double itv  = diagram->getGridInterval( canvas );
        x0 = Utils::rndToNearestMult( xCtr+x0, itv)-xCtr;
    }

    s->getDOFXCtr()->setMax( x0 );
}


// -------------------------------------------------
void DiagramEditor::handleDragDOFYCtrBeg( Shape* s )
// -------------------------------------------------
{
    double xCtr, yCtr;    // center of shape
    double xCur, yCur;    // current mouse position
    double y0;            // y position after translating to shape's center

    // get shape's geometry
    s->getCenter( xCtr, yCtr );
    
    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );

    // translate to center, the 'origin'
    y0 = yCur-yCtr;

    if ( diagram->getSnapGrid() == true )
    {
        double itv  = diagram->getGridInterval( canvas );
        y0 = Utils::rndToNearestMult( yCtr+y0, itv)-yCtr;
    }

    s->getDOFYCtr()->setMin( y0 );
}


// -------------------------------------------------
void DiagramEditor::handleDragDOFYCtrEnd( Shape* s )
// -------------------------------------------------
{
    double xCtr, yCtr;    // center of shape
    double xCur, yCur;    // current mouse position
    double y0;            // y position after translating to shape's center

    // get shape's geometry
    s->getCenter( xCtr, yCtr );
    
    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );

    // translate to center, the 'origin'
    y0 = yCur-yCtr;

    if ( diagram->getSnapGrid() == true )
    {
        double itv  = diagram->getGridInterval( canvas );
        y0 = Utils::rndToNearestMult( yCtr+y0, itv)-yCtr;
    }

    s->getDOFYCtr()->setMax( y0 );
}


// ------------------------------------------------
void DiagramEditor::handleDragDOFWthBeg( Shape* s )
// ------------------------------------------------
{
    double angl;          // shape's rotation angle in radians
    double xCtr, yCtr;    // center of shape
    double xDFC, yDFC;    // deltas of shape
    double xCur, yCur;    // current mouse position
    double x0,   y0;      // position after translating to shape's center
    double xS,   yS;      // position after rotating to shape's angle
    double hyp;           // hypotenuse, for movement parallel to shape's x-axis

    // get shape's geometry
    angl = Utils::degrToRad( s->getAngleCtr() );
    s->getCenter( xCtr, yCtr );
    s->getDFC( xDFC, yDFC );

    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );

    // translate to center, the 'origin'
    x0 = xCur-xCtr;
    y0 = yCur-yCtr;

    // rotate to 'normal' orientation, find x & y
    xS = x0*cos( -angl ) - y0*sin( -angl );
    yS = x0*sin( -angl ) + y0*cos( -angl );

    if ( diagram->getSnapGrid() == true )
    {
        double itv  = diagram->getGridInterval( canvas );
        double a = Utils::rndToNearestMult( xCtr+xS*cos(angl), itv )-xCtr;
        xS = a/cos(angl);
    }

    hyp = xS-xDFC;
    s->getDOFWth()->setMin( hyp );
}


// ------------------------------------------------
void DiagramEditor::handleDragDOFWthEnd( Shape* s )
// ------------------------------------------------
{
    double angl;          // shape's rotation angle in radians
    double xCtr, yCtr;    // center of shape
    double xDFC, yDFC;    // deltas of shape
    double xCur, yCur;    // current mouse position
    double x0,   y0;      // position after translating to shape's center
    double xS,   yS;      // position after rotating to shape's angle
    double hyp;           // hypotenuse, for movement parallel to shape's x-axis

    // get shape's geometry
    angl = Utils::degrToRad( s->getAngleCtr() );
    s->getCenter( xCtr, yCtr );
    s->getDFC( xDFC, yDFC );

    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );

    // translate to center, the 'origin'
    x0 = xCur-xCtr;
    y0 = yCur-yCtr;

    // rotate to 'normal' orientation, find x & y
    xS = x0*cos( -angl ) - y0*sin( -angl );
    yS = x0*sin( -angl ) + y0*cos( -angl );

    if ( diagram->getSnapGrid() == true )
    {
        double itv  = diagram->getGridInterval( canvas );
        double a = Utils::rndToNearestMult( xCtr+xS*cos(angl), itv )-xCtr;
        xS = a/cos(angl);
    }

    hyp = xS-xDFC;
    s->getDOFWth()->setMax( hyp );
}


// ------------------------------------------------
void DiagramEditor::handleDragDOFHgtBeg( Shape* s )
// ------------------------------------------------
{
    double angl;          // shape's rotation angle in radians
    double xCtr, yCtr;    // center of shape
    double xDFC, yDFC;    // deltas of shape
    double xCur, yCur;    // current mouse position
    double x0,   y0;      // position after translating to shape's center
    double xS,   yS;      // position after rotating to shape's angle
    double hyp;           // hypotenuse, for movement parallel to shape's y-axis

    // get shape's geometry
    angl = Utils::degrToRad( s->getAngleCtr() );
    s->getCenter( xCtr, yCtr );
    s->getDFC( xDFC, yDFC );

    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );
    
    // translate to xCtr, the 'origin'
    x0 = xCur-xCtr;
    y0 = yCur-yCtr;
    // rotate to 'normal' orientation, find x & y
    xS = x0*cos( -angl ) - y0*sin( -angl );
    yS = x0*sin( -angl ) + y0*cos( -angl );

    if ( diagram->getSnapGrid() == true )
    {
        double itv  = diagram->getGridInterval( canvas );
        double a = Utils::rndToNearestMult( yCtr+yS*cos(angl), itv )-yCtr;
        yS = a/cos(angl);
    }

    hyp = yS-yDFC;  
    s->getDOFHgt()->setMin( hyp );
}


// ------------------------------------------------
void DiagramEditor::handleDragDOFHgtEnd( Shape* s )
// ------------------------------------------------
{
    double angl;          // shape's rotation angle in radians
    double xCtr, yCtr;    // center of shape
    double xDFC, yDFC;    // deltas of shape
    double xCur, yCur;    // current mouse position
    double x0,   y0;      // position after translating to shape's center
    double xS,   yS;      // position after rotating to shape's angle
    double hyp;           // hypotenuse, for movement parallel to shape's y-axis

    // get shape's geometry
    angl = Utils::degrToRad( s->getAngleCtr() );
    s->getCenter( xCtr, yCtr );
    s->getDFC( xDFC, yDFC );

    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );
    
    // translate to xCtr, the 'origin'
    x0 = xCur-xCtr;
    y0 = yCur-yCtr;
    // rotate to 'normal' orientation, find x & y
    xS = x0*cos( -angl ) - y0*sin( -angl );
    yS = x0*sin( -angl ) + y0*cos( -angl );

    if ( diagram->getSnapGrid() == true )
    {
        double itv  = diagram->getGridInterval( canvas );
        double a = Utils::rndToNearestMult( yCtr+yS*cos(angl), itv )-yCtr;
        yS = a/cos(angl);
    }

    hyp = yS-yDFC;  
    s->getDOFHgt()->setMax( hyp );
}


// ------------------------------------------
void DiagramEditor::handleDragHge( Shape* s )
// ------------------------------------------
{
    double xCtr, yCtr;    // center of shape
    double xCur, yCur;    // current mouse position
    double x0,   y0;      // y position after translating to shape's center

    // get shape's geometry
    s->getCenter( xCtr, yCtr );
    
    // get mouse info
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );

    // translate to center, the 'origin'
    y0 = yCur-yCtr;
    x0 = xCur-xCtr;

    if ( diagram->getSnapGrid() == true )
    {
        double itv  = diagram->getGridInterval( canvas );
        x0 = Utils::rndToNearestMult( xCtr+x0, itv)-xCtr;
        y0 = Utils::rndToNearestMult( yCtr+y0, itv)-yCtr;
    }

    s->setHinge( x0, y0 );
}


// ------------------------------------------------
void DiagramEditor::handleDragDOFAglBeg( Shape* s )
// ------------------------------------------------
{
    double xHge, yHge, xCtr, yCtr;
    double dstHgeCtr;
    double aglRef, aglTot;
    double xCur, yCur, xRelHge, yRelHge;
        
    // distance from hinge to center
    s->getHinge( xHge, yHge );
    s->getCenter( xCtr, yCtr );
    dstHgeCtr = Utils::dist( xCtr+xHge, yCtr+yHge, xCtr, yCtr );
    // angle center relative to hinge    
    if ( dstHgeCtr == 0 )
        aglRef = 0;
    else
        aglRef = Utils::calcAngleDg( -xHge, -yHge );

    // mouse position relative to hinge
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );
    xRelHge = xCur-(xCtr+xHge);
    yRelHge = yCur-(yCtr+yHge);

    aglTot = Utils::calcAngleDg( xRelHge, yRelHge );
    
    if ( diagram->getSnapGrid() == true )
    {
        double itvAgl  = diagram->getAngleInterval();
        aglTot = Utils::rndToNearestMult( aglTot, itvAgl);
    }

    s->getDOFAgl()->setMin( aglTot-aglRef );
}


// ------------------------------------------------
void DiagramEditor::handleDragDOFAglEnd( Shape* s )
// ------------------------------------------------
{
    double xHge, yHge, xCtr, yCtr;
    double dstHgeCtr;
    double aglRef, aglTot;
    double xCur, yCur, xRelHge, yRelHge;
        
    // distance from hinge to center
    s->getHinge( xHge, yHge );
    s->getCenter( xCtr, yCtr );
    dstHgeCtr = Utils::dist( xCtr+xHge, yCtr+yHge, xCtr, yCtr );
    // angle center relative to hinge    
    if ( dstHgeCtr == 0 )
        aglRef = 0;
    else
        aglRef = Utils::calcAngleDg( -xHge, -yHge );

    // mouse position relative to hinge
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  xCur, yCur );

    
    xRelHge = xCur-(xCtr+xHge);
    yRelHge = yCur-(yCtr+yHge);

    /*
    // rotate to 'normal' orientation, find x & y
    double xTmp, yTmp;
    xS = x0*cos( -angl ) - y0*sin( -angl );
    yS = x0*sin( -angl ) + y0*cos( -angl );
    */
    
    aglTot = Utils::calcAngleDg( xRelHge, yRelHge );
    
    if ( diagram->getSnapGrid() == true )
    {
        double itvAgl  = diagram->getAngleInterval();
        aglTot = Utils::rndToNearestMult( aglTot, itvAgl);
    }

    s->getDOFAgl()->setMax( aglTot-aglRef );
}


// -- hit detection -------------------------------------------------


// -----------------------------
void DiagramEditor::processHits(  
    GLint hits, 
    GLuint buffer[] )
// -----------------------------
{
    GLuint *ptr;
    int number;
    vector< int > ids;
    
    ptr = (GLuint*) buffer;

    if ( hits > 0 )
    {
        // if necassary, advance to closest hit
        if ( hits > 1 )
        {
            for ( int i = 0; i < ( hits-1 ); ++i )
            {
                number = *ptr;
                ++ptr; // number;
                ++ptr; // z1
                ++ptr; // z2
                for ( int j = 0; j < number; ++j )
                    ++ptr; // names
            }
        }

        // last hit
        number = *ptr;
        ++ptr; // number
        ++ptr; // z1
        ++ptr; // z2

        for ( int i = 0; i < number; ++i )
        {
            ids.push_back( *ptr );
            ++ptr;
        }

        handleHits( ids );
    }   

    ptr = NULL;
}


// -- end -----------------------------------------------------------
