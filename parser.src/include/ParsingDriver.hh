#ifndef _PARSING_DRIVER_HH
#define _PARSING_DRIVER_HH

#include <iostream>
#include <vector>

#include <math.h>

#include "ModFile.hh"
#include "TmpSymbolTable.hh"
#include "DynareBison.hh"
#include "ComputingTasks.hh"
#include "Shocks.hh"
#include "SigmaeInitialization.hh"
#include "NumericalInitialization.hh"
#include "ModelTree.hh"

using namespace std;

// Announce to Flex the prototype we want for lexing function, ...
#define YY_DECL                                              \
  yy::parser::token_type                                     \
  yylex(yy::parser::semantic_type *yylval,                   \
        yy::parser::location_type *yylloc,                   \
        ParsingDriver &driver)
// ... and declare it for the parser's sake.
YY_DECL;

//! Drives the scanning and parsing of the .mod file, and constructs its abstract representation
/*! It is built along the guidelines given in Bison 2.3 manual. */
class ParsingDriver
{
private:
  //! Start scanning
  /*! Body defined at the end of DynareFlex.ll, for convenience reasons. */
  void scan_begin();

  //! Stop scanning
  /*! Body defined at the end of DynareFlex.ll, for convenience reasons. */
  void scan_end();

  //! Checks that a given symbol exists, and stops with an error message if it doesn't
  void check_symbol_existence(const string &name);

  //! Helper to add a symbol declaration
  void declare_symbol(string *name, Type type, string *tex_name);

  //! Creates option "optim_opt" in OptionsList if it doesn't exist, else add a comma, and adds the option name
  void optim_options_helper(const string &name);

  //! Stores temporary symbol table
  TmpSymbolTable *tmp_symbol_table;

  //! The data tree in which to add expressions currently parsed
  DataTree *data_tree;

  //! The model tree in which to add expressions currently parsed
  /*! It is only a dynamic cast of data_tree pointer, and is therefore null if data_tree is not a ModelTree instance */
  ModelTree *model_tree;

  //! Sets data_tree and model_tree pointers
  void set_current_data_tree(DataTree *data_tree_arg);

  //! Stores options lists
  OptionsList options_list;
  //! Temporary storage for trend elements
  ObservationTrendsStatement::trend_elements_type trend_elements;
  //! Temporary storage for filename list of ModelComparison
  ModelComparisonStatement::filename_list_type filename_list;
  //! Temporary storage for list of EstimationParams (from estimated_params* statements)
  vector<EstimationParams> estim_params_list;
  //! Temporary storage of variances from optim_weights
  OptimWeightsStatement::var_weights_type var_weights;
  //! Temporary storage of covariances from optim_weights
  OptimWeightsStatement::covar_weights_type covar_weights;
  //! Temporary storage of variances from calib_var
  CalibVarStatement::calib_var_type calib_var;
  //! Temporary storage of covariances from calib_var
  CalibVarStatement::calib_covar_type calib_covar;
  //! Temporary storage of autocorrelations from calib_var
  CalibVarStatement::calib_ac_type calib_ac;
  //! Temporary storage for deterministic shocks
  ShocksStatement::det_shocks_type det_shocks;
  //! Temporary storage for periods of deterministic shocks
  vector<pair<int, int> > det_shocks_periods;
  //! Temporary storage for values of deterministic shocks
  vector<NodeID> det_shocks_values;
  //! Temporary storage for variances of shocks
  ShocksStatement::var_and_std_shocks_type var_shocks;
  //! Temporary storage for standard errors of shocks
  ShocksStatement::var_and_std_shocks_type std_shocks;
  //! Temporary storage for covariances of shocks
  ShocksStatement::covar_and_corr_shocks_type covar_shocks;
  //! Temporary storage for correlations of shocks
  ShocksStatement::covar_and_corr_shocks_type corr_shocks;
  //! Temporary storage for Sigma_e rows
  SigmaeStatement::row_type sigmae_row;
  //! Temporary storage for Sigma_e matrix
  SigmaeStatement::matrix_type sigmae_matrix;
  //! Temporary storage for initval/endval blocks
  InitOrEndValStatement::init_values_type init_values;
  //! Temporary storage for histval blocks
  HistValStatement::hist_values_type hist_values;
  //! Temporary storage for homotopy_setup blocks
  HomotopyStatement::homotopy_values_type homotopy_values;

  //! Temporary storage for argument list of unknown function
  vector<NodeID> unknown_function_args;

  //! The mod file representation constructed by this ParsingDriver
  ModFile *mod_file;

public:
  //! Constructor
  ParsingDriver();
  //! Destructor
  virtual ~ParsingDriver();

  //! Starts parsing, and constructs the MOD file representation
  /*! \param f Name of file to parse

      The returned pointer should be deleted after use.
   */
  ModFile *parse(const string &f);

  //! Name of file being parsed
  string file;

  //! Trace scanning ?
  /*! If set to true before calling parse(), the flex scanner will dump a lot of debugging information. Defaults to false.
  */
  bool trace_scanning;

  //! Trace parsing ?
  /*! If set to true before calling parse(), the bison parser will dump debugging information. Defaults to false. */
  bool trace_parsing;

  //! Estimation parameters
  EstimationParams estim_params;

  //! Error handler with location
  void error(const yy::parser::location_type &l, const string &m);
  //! Error handler without location
  void error(const string &m);
  //! Warning handler
  void warning(const string &m);

  //! Check if a given symbol exists in the parsing context, and is not a mod file local variable
  bool symbol_exists_and_is_not_modfile_local_variable(const char *s);
  //! Sets mode of ModelTree class to use C output
  void use_dll();
  //! Sets mode of ModelTree class to block decompose the model and triggers the creation of the incidence matrix in a C context
  void sparse_dll();
  //! Sets mode of ModelTree class to block decompose the model and triggers the creation of the incidence matrix in Matlab context
  void sparse();
  //! Sets the compiler type used in conjunction with SPARCE_DLL
  void init_compiler(int compiler_type);
  //! Sets the FILENAME for the initial value in initval
  void init_val_filename(string *filename);
  //! Declares an endogenous variable
  void declare_endogenous(string *name, string *tex_name = new string);
  //! Declares an exogenous variable
  void declare_exogenous(string *name, string *tex_name = new string);
  //! Declares an exogenous deterministic variable
  void declare_exogenous_det(string *name, string *tex_name = new string);
  //! Declares a parameter
  void declare_parameter(string *name, string *tex_name = new string);
  //! Declares and initializes a local parameter
  void declare_and_init_model_local_variable(string *name, NodeID rhs);
  //! Adds a constant to DataTree
  NodeID add_constant(string *constant);
  //! Adds a model variable to ModelTree and VariableTable
  NodeID add_model_variable(string *name);
  //! Adds a model lagged variable to ModelTree and VariableTable
  NodeID add_model_variable(string *name, string *olag);
  //! Adds an Expression's variable
  NodeID add_expression_variable(string *name);
  //! Adds a "periods" statement
  void periods(string *periods);
  //! Adds a "cutoff" statement
  void cutoff(string *cutoff);
  //! Adds a weight of the "markowitz" criteria statement
  void markowitz(string *markowitz);
  //! Adds a "dsample" statement
  void dsample(string *arg1);
  //! Adds a "dsample" statement
  void dsample(string *arg1, string *arg2);
  //! Writes parameter intitialisation expression
  void init_param(string *name, NodeID rhs);
  //! Writes an initval block
  void init_val(string *name, NodeID rhs);
  //! Writes an histval block
  void hist_val(string *name, string *lag, NodeID rhs);
  //! Writes an homotopy_setup block
  void homotopy_val(string *name, NodeID val1, NodeID val2);
  //! Writes end of an initval block
  void end_initval();
  //! Writes end of an endval block
  void end_endval();
  //! Writes end of an histval block
  void end_histval();
  //! Writes end of an homotopy_setup block
  void end_homotopy();
  //! Begin a model block
  void begin_model();
  //! Writes a shocks statement
  void end_shocks();
  //! Writes a mshocks statement
  void end_mshocks();
  //! Adds a deterministic chock
  void add_det_shock(string *var);
  //! Adds a std error chock
  void add_stderr_shock(string *var, NodeID value);
  //! Adds a variance chock
  void add_var_shock(string *var, NodeID value);
  //! Adds a covariance chock
  void add_covar_shock(string *var1, string *var2, NodeID value);
  //! Adds a correlated chock
  void add_correl_shock(string *var1, string *var2, NodeID value);
  //! Adds a shock period range
  void add_period(string *p1, string *p2);
  //! Adds a shock period
  void add_period(string *p1);
  //! Adds a deterministic shock value
  void add_value(NodeID value);
  //! Adds a deterministic shock value
  void add_value(string *p1);
  //! Writes a Sigma_e block
  void do_sigma_e();
  //! Ends row of Sigma_e block
  void end_of_row();
  //! Adds a constant element to current row of Sigma_e
  void add_to_row_const(string *s);
  //! Adds an expression element to current row of Sigma_e
  void add_to_row(NodeID v);
  //! Write a steady command
  void steady();
  //! Sets an option to a numerical value
  void option_num(const string &name_option, string *opt);
  //! Sets an option to a numerical value
  void option_num(const string &name_option, const string &opt);
  //! Sets an option to a numerical value
  void option_num(const string &name_option, string *opt1, string *opt2);
  //! Sets an option to a string value
  void option_str(const string &name_option, string *opt);
  //! Sets an option to a string value
  void option_str(const string &name_option, const string &opt);
  //! Indicates that the model is linear
  void linear();
  //! Adds a variable to temp symbol table and sets its value
  void add_tmp_var(string *tmp_var1, string *tmp_var2);
  //! Adds a variable to temp symbol table
  void add_tmp_var(string *tmp_var);
  //! Writes a rplot() command
  void rplot();
  //! Writes a stock_simul command
  void stoch_simul();
  //! Determine whether to write simul command or simul_sparse command
  void simulate();
  //! Writes a simul_sparse command
  void simul_sparse();
  //! Writes a simul command
  void simul();
  //! Writes check command
  void check();
  //! Writes estimated params command
  void estimated_params();
  //! Writes estimated params init command
  void estimated_params_init();
  //! Writes estimated params bound command
  void estimated_params_bounds();
  //! Add a line in an estimated params block
  void add_estimated_params_element();
  //! Runs estimation process
  void run_estimation();
  //! Runs prior_analysis();
  void run_prior_analysis();
  //! Runs posterior_analysis();
  void run_posterior_analysis();
  //! Runs dynare_sensitivy()
  void dynare_sensitivity();
  //! Adds an optimization option (string value)
  void optim_options_string(string *name, string *value);
  //! Adds an optimization option (numeric value)
  void optim_options_num(string *name, string *value);
  //! Prints varops instructions
  void set_varobs();
  void set_trends();
  void set_trend_element(string *arg1, NodeID arg2);
  void set_unit_root_vars();
  void optim_weights();
  void set_optim_weights(string *name, NodeID value);
  void set_optim_weights(string *name1, string *name2, NodeID value);
  void set_osr_params();
  void run_osr();
  void run_calib_var();
  void set_calib_var(string *name, string *weight, NodeID expression);
  void set_calib_covar(string *name1, string *name2, string *weight, NodeID expression);
  void set_calib_ac(string *name, string *ar, string *weight, NodeID expression);
  void run_calib(int covar);
  void run_dynasave(string *arg1, string *arg2 = new string);
  void run_dynatype(string *arg1, string *arg2 = new string);
  void add_mc_filename(string *filename, string *prior = new string("1"));
  void run_model_comparison();
  //! Begin a planner_objective statement
  void begin_planner_objective();
  //! End a planner objective statement
  void end_planner_objective(NodeID expr);
  //! ramsey policy statement
  void ramsey_policy();
  //! BVAR marginal density
  void bvar_density(string *maxnlags);
  //! BVAR forecast
  void bvar_forecast(string *nlags);
  //! Writes token "arg1=arg2" to model tree
  NodeID add_model_equal(NodeID arg1, NodeID arg2);
  //! Writes token "arg=0" to model tree
  NodeID add_model_equal_with_zero_rhs(NodeID arg);
  //! Writes token "arg1+arg2" to model tree
  NodeID add_plus(NodeID arg1, NodeID arg2);
  //! Writes token "arg1-arg2" to model tree
  NodeID add_minus(NodeID arg1,  NodeID arg2);
  //! Writes token "-arg1" to model tree
  NodeID add_uminus(NodeID arg1);
  //! Writes token "arg1*arg2" to model tree
  NodeID add_times(NodeID arg1,  NodeID arg2);
  //! Writes token "arg1/arg2" to model tree
  NodeID add_divide(NodeID arg1,  NodeID arg2);
  //! Writes token "arg1<arg2" to model tree
  NodeID add_less(NodeID arg1, NodeID arg2);
  //! Writes token "arg1>arg2" to model treeNodeID
  NodeID add_greater(NodeID arg1, NodeID arg2);
  //! Writes token "arg1<=arg2" to model treeNodeID
  NodeID add_less_equal(NodeID arg1, NodeID arg2);
  //! Writes token "arg1>=arg2" to model treeNodeID
  NodeID add_greater_equal(NodeID arg1, NodeID arg2);
  //! Writes token "arg1==arg2" to model treeNodeIDNodeID
  NodeID add_equal_equal(NodeID arg1, NodeID arg2);
  //! Writes token "arg1!=arg2" to model treeNodeIDNodeID
  NodeID add_different(NodeID arg1, NodeID arg2);
  //! Writes token "arg1^arg2" to model tree
  NodeID add_power(NodeID arg1,  NodeID arg2);
  //! Writes token "exp(arg1)" to model tree
  NodeID add_exp(NodeID arg1);
  //! Writes token "log(arg1)" to model tree
  NodeID add_log(NodeID arg1);
  //! Writes token "log10(arg1)" to model tree
  NodeID add_log10(NodeID arg1);
  //! Writes token "cos(arg1)" to model tree
  NodeID add_cos(NodeID arg1);
  //! Writes token "sin(arg1)" to model tree
  NodeID add_sin(NodeID arg1);
  //! Writes token "tan(arg1)" to model tree
  NodeID add_tan(NodeID arg1);
  //! Writes token "acos(arg1)" to model tree
  NodeID add_acos(NodeID arg1);
  //! Writes token "asin(arg1)" to model tree
  NodeID add_asin(NodeID arg1);
  //! Writes token "atan(arg1)" to model tree
  NodeID add_atan(NodeID arg1);
  //! Writes token "cosh(arg1)" to model tree
  NodeID add_cosh(NodeID arg1);
  //! Writes token "sinh(arg1)" to model tree
  NodeID add_sinh(NodeID arg1);
  //! Writes token "tanh(arg1)" to model tree
  NodeID add_tanh(NodeID arg1);
  //! Writes token "acosh(arg1)" to model tree
  NodeID add_acosh(NodeID arg1);
  //! Writes token "asin(arg1)" to model tree
  NodeID add_asinh(NodeID arg1);
  //! Writes token "atanh(arg1)" to model tree
  NodeID add_atanh(NodeID arg1);
  //! Writes token "sqrt(arg1)" to model tree
  NodeID add_sqrt(NodeID arg1);
  //! Writes token "max(arg1,arg2)" to model tree
  NodeID add_max(NodeID arg1, NodeID arg2);
  //! Writes token "min(arg1,arg2)" to model tree
  NodeID add_min(NodeID arg1, NodeID arg2);
  //! Writes token "normcdf(arg1,arg2,arg3)" to model tree
  NodeID add_normcdf(NodeID arg1, NodeID arg2, NodeID arg3);
  //! Adds an unknwon function argument
  void add_unknown_function_arg(NodeID arg);
  //! Adds an unknown function call node
  NodeID add_unknown_function(string *function_name);
  //! Adds a native statement
  void add_native(const char *s);
  //! Resets data_tree and model_tree pointers to default (i.e. mod_file->expressions_tree)
  void reset_data_tree();
};

#endif // ! PARSING_DRIVER_HH
