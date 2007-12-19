#include <algorithm>
#include <sstream>

#include "SymbolTable.hh"
#include "Interface.hh"

SymbolTable::SymbolTable() : endo_nbr(0), exo_nbr(0), exo_det_nbr(0), parameter_nbr(0),
                             model_local_variable_nbr(0), modfile_local_variable_nbr(0),
                             recur_nbr(0), unknown_function_nbr(0)
{
}

void
SymbolTable::addSymbol(const string &name, Type type, const string &tex_name) throw (AlreadyDeclaredException)
{
  if (exists(name))
    if (symbol_table[name].first == type)
      throw AlreadyDeclaredException(name, true);
    else
      throw AlreadyDeclaredException(name, false);

  int id;

  switch (type)
    {
    case eExogenous:
      id = exo_nbr++;
      break;
    case eExogenousDet:
      id = exo_det_nbr++;
      break;
    case eEndogenous:
      id = endo_nbr++;
      break;
    case eParameter:
      id = parameter_nbr++;
      break;
    case eRecursiveVariable:
      id = recur_nbr++;
      break;
    case eModelLocalVariable:
      id = model_local_variable_nbr++;
      break;
    case eModFileLocalVariable:
      id = modfile_local_variable_nbr++;
      break;
    case eUnknownFunction:
      id = unknown_function_nbr++;
      break;
    }

  symbol_type symbol(type, id); 
  symbol_table[name] = symbol;
  name_table[symbol] = name;
  tex_name_table[symbol] = tex_name;
}

void
SymbolTable::writeOutput(ostream &output) const
{
  if (exo_nbr > 0)
    {
      output << "M_.exo_names = '" << getNameByID(eExogenous, 0) << "';" << endl;
      output << "M_.exo_names_tex = '" << getTeXNameByID(eExogenous, 0) << "';" << endl;
      for (int id = 1; id < exo_nbr; id++)
        {
          output << "M_.exo_names = " << interfaces::strvcat("M_.exo_names","'"+getNameByID(eExogenous, id)+"'") << ";" << endl;
          output << "M_.exo_names_tex = " << interfaces::strvcat("M_.exo_names_tex","'"+getTeXNameByID(eExogenous, id)+"'") << ";" << endl;
        }
    }
  if (exo_det_nbr > 0)
    {
      output << "lgxdet_ = '" << getNameByID(eExogenousDet, 0) << "';" << endl;
      output << "lgxdet_tex_ = '" << getTeXNameByID(eExogenousDet, 0) << "';" << endl;
      for (int id = 1; id < exo_det_nbr; id++)
        {
          output << "lgxdet_ = " << interfaces::strvcat("lgxdet_","'"+getNameByID(eExogenousDet, id)+"'") << ";" << endl;
          output << "lgxdet_tex_ = " << interfaces::strvcat("lgxdet_tex_","'"+getTeXNameByID(eExogenousDet, id)+"'") << ";" << endl;
        }
    }
  if (endo_nbr > 0)
    {
      output << "M_.endo_names = '" << getNameByID(eEndogenous, 0) << "';" << endl;
      output << "M_.endo_names_tex = '" << getTeXNameByID(eEndogenous, 0) << "';" << endl;
      for (int id = 1; id < endo_nbr; id++)
        {
          output << "M_.endo_names = " << interfaces::strvcat("M_.endo_names","'"+getNameByID(eEndogenous, id)+"'") << ";" << endl;
          output << "M_.endo_names_tex = " << interfaces::strvcat("M_.endo_names_tex","'"+getTeXNameByID(eEndogenous, id)+"'") << ";" << endl;
        }
    }
  if (recur_nbr > 0)
    {
      output << "M_.recur_names = '" << getNameByID(eRecursiveVariable, 0) << "';" << endl;
      output << "M_.recur_names_tex = '" << getTeXNameByID(eRecursiveVariable, 0) << "';" << endl;
      for (int id = 1; id < recur_nbr; id++)
        {
          output << "M_.recur_names = " << interfaces::strvcat("M_.recur_names","'"+getNameByID(eRecursiveVariable, id)+"'") << ";" << endl;
          output << "M_.recur_names_tex = " << interfaces::strvcat("M_.recur_names_tex","'"+getTeXNameByID(eRecursiveVariable, id)+"'") << ";" << endl;
        }
    }
  if (parameter_nbr > 0)
    {
      output << "M_.param_names = '" << getNameByID(eParameter, 0) << "';" << endl;
      output << "M_.param_names_tex = '" << getTeXNameByID(eParameter, 0) << "';" << endl;
      for (int id = 1; id < parameter_nbr; id++)
        {
          output << "M_.param_names = " << interfaces::strvcat("M_.param_names","'"+getNameByID(eParameter, id)+"'") << ";" << endl;
          output << "M_.param_names_tex = " << interfaces::strvcat("M_.param_names_tex","'"+getTeXNameByID(eParameter, id)+"'") << ";" << endl;
        }
    }

  output << "M_.exo_det_nbr = " << exo_det_nbr << ";" << endl
         << "M_.exo_nbr = " << exo_nbr << ";" << endl
         << "M_.endo_nbr = " << endo_nbr << ";" << endl
         << "M_.recur_nbr = " << recur_nbr << ";" << endl
         << "M_.param_nbr = " << parameter_nbr << ";" << endl;

  output << "M_.Sigma_e = zeros(" << exo_nbr << ", " << exo_nbr << ");" << endl;
}
