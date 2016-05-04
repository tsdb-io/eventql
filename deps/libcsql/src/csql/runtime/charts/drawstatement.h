/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <eventql/util/stdtypes.h>
#include <csql/tasks/Task.h>
#include <csql/qtree/DrawStatementNode.h>
#include <csql/parser/token.h>
#include <csql/Transaction.h>
#include <eventql/util/exception.h>
#include <eventql/util/autoref.h>
#include <cplot/canvas.h>
#include <cplot/drawable.h>

namespace csql {
class Runtime;

class DrawStatement : public Statement {
public:

  DrawStatement(
      Transaction* ctx,
      RefPtr<DrawStatementNode> node,
      Vector<ScopedPtr<Task>> sources,
      Runtime* runtime);

protected:

  template <typename ChartBuilderType>
  stx::chart::Drawable* executeWithChart(
      ExecutionContext* context,
      stx::chart::Canvas* canvas) {
    ChartBuilderType chart_builder(canvas, node_);

    for (auto& source : sources_) {
      chart_builder.executeStatement(source.get(), context);
    }

    return chart_builder.getChart();
  }

  void applyAxisDefinitions(stx::chart::Drawable* chart) const;
  void applyAxisLabels(ASTNode* ast, stx::chart::AxisDefinition* axis) const;
  void applyDomainDefinitions(stx::chart::Drawable* chart) const;
  void applyGrid(stx::chart::Drawable* chart) const;
  void applyLegend(stx::chart::Drawable* chart) const;
  void applyTitle(stx::chart::Drawable* chart) const;

  Transaction* ctx_;
  RefPtr<DrawStatementNode> node_;
  Vector<ScopedPtr<Task>> sources_;
  Runtime* runtime_;
};

}
