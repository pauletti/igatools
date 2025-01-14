//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2016  by the igatools authors (see authors.txt).
//
// This file is part of the igatools library.
//
// The igatools library is free software: you can use it, redistribute
// it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//-+--------------------------------------------------------------------

#include <IgatoolsParaViewReader.h>

#include <vtkObjectFactory.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkMultiBlockDataSet.h>

#include <paraview_plugin/vtk_iga_grid_container.h>

using std::string;

vtkStandardNewMacro(IgatoolsParaViewReader);


#ifndef IGATOOLS_WITH_SERIALIZATION
#ifndef IGATOOLS_WITH_XML_IO
static_assert(true, "Neither serialization nor XML capabilities are active.");
#endif
#endif

IgatoolsParaViewReader::IgatoolsParaViewReader()
  :
  n_vis_elem_phys_solid_(1),
  n_vis_elem_parm_solid_(1),
  n_vis_elem_phys_knot_(1),
  n_vis_elem_parm_knot_(1),
  iga_grid_container_(iga::paraview_plugin::VtkIgaGridContainer::create_void())
{
#ifndef NDEBUG
  this->DebugOn();
#else
  this->DebugOff();
#endif

  this->SetNumberOfInputPorts(0); // No vtk input, this is not a filter.
  this->SetNumberOfOutputPorts(1); // Just one output, a multi block.
}



const char *
IgatoolsParaViewReader::
GetClassNameInternal() const
{
  return "IgatoolsParaViewReader";
}



int
IgatoolsParaViewReader::
IsTypeOf(const char *type)
{
  if (!strcmp("IgatoolsParaViewReader", type))
    return 1;
  return vtkMultiBlockDataSetAlgorithm::IsTypeOf(type);
}



int
IgatoolsParaViewReader::
IsA(const char *type)
{
  return this->IgatoolsParaViewReader::IsTypeOf(type);
}



IgatoolsParaViewReader *
IgatoolsParaViewReader::
SafeDownCast(vtkObjectBase *o)
{
  if (o && o->IsA("IgatoolsParaViewReader"))
    return static_cast<IgatoolsParaViewReader *>(o);
  return NULL;
}



IgatoolsParaViewReader *
IgatoolsParaViewReader::
NewInstance() const
{
  return IgatoolsParaViewReader::SafeDownCast(this->NewInstanceInternal());
}



vtkObjectBase *
IgatoolsParaViewReader::
NewInstanceInternal() const
{
  return IgatoolsParaViewReader::New();
}



int IgatoolsParaViewReader::RequestInformation(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(input_vec),
  vtkInformationVector *output_vec)
{
  vtkInformation *info = output_vec->GetInformationObject(0);

  if (!vtkMultiBlockDataSet::SafeDownCast(info->Get(vtkDataObject::DATA_OBJECT())))
    return 0;

  // If the file is not parse, it is parsed now.
  if (!parse_input_file_)
    return 1;

  try
  {
    this->SetProgressText("Parsing igatools file.");

    iga_grid_container_ = iga::paraview_plugin::VtkIgaGridContainer::create
                          (file_name_,
                           n_vis_elem_phys_solid_, phys_sol_grid_type_,
                           n_vis_elem_phys_knot_,  phys_knt_grid_type_,
                           phys_ctr_grid_type_,
                           n_vis_elem_parm_solid_, parm_sol_grid_type_,
                           n_vis_elem_parm_knot_,  parm_knt_grid_type_);

    parse_input_file_ = false;

    return 1;
  }
  catch (std::exception &e)
  {
    VtkIgaErrorMacro("Parsing file " << string(file_name_) << ":\n"
                     << e.what());

    iga_grid_container_ = iga::paraview_plugin::VtkIgaGridContainer::create_void();

    return 0;
  }
  catch (...)
  {
    VtkIgaErrorMacro("Parsing file " << string(file_name_) << ":\n"
                     << "AN UNKNOWN EXCEPTION OCCUR!");

    iga_grid_container_ = iga::paraview_plugin::VtkIgaGridContainer::create_void();

    return 0;
  }

}



int IgatoolsParaViewReader::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(input_vec),
  vtkInformationVector *output_vec)
{
  vtkInformation *info = output_vec->GetInformationObject(0);
  vtkMultiBlockDataSet *output =
    vtkMultiBlockDataSet::SafeDownCast(info->Get(vtkDataObject::DATA_OBJECT()));

  this->SetProgressText("Generating igatools geometries.");

  this->UpdateProgress(0.0);

  iga_grid_container_->update(n_vis_elem_phys_solid_, phys_sol_grid_type_,
                              n_vis_elem_phys_knot_,  phys_knt_grid_type_,
                              phys_ctr_grid_type_,
                              n_vis_elem_parm_solid_, parm_sol_grid_type_,
                              n_vis_elem_parm_knot_,  parm_knt_grid_type_);

  try
  {
    iga_grid_container_->create_multiblock_grid(create_physical_mesh_,
                                                create_sol_mesh_phys_,
                                                create_knt_mesh_phys_,
                                                create_ctr_mesh_phys_,
                                                create_parametric_mesh_,
                                                create_sol_mesh_parm_,
                                                create_knt_mesh_parm_,
                                                output);
    return 1;
  }
  catch (std::exception &exc)
  {
    VtkIgaErrorMacro(<< exc.what());
    return 0;
  }
  catch (...)
  {
    VtkIgaErrorMacro("AN UNKNOWN EXCEPTION OCCUR!");
    return 0;
  }
}



void IgatoolsParaViewReader::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "File Name: "
     << (this->file_name_ ? this->file_name_ : "(none)") << "\n";
}



int
IgatoolsParaViewReader::
CanReadFile(const char *name)
{
  try
  {
    iga::paraview_plugin::VtkIgaGridContainer::check_file(name);
    return 1;
  }
  catch (std::exception &e)
  {
    VtkIgaErrorMacro("Parsing file " << string(file_name_) << ":\n"
                     << e.what());

    iga_grid_container_ = iga::paraview_plugin::VtkIgaGridContainer::create_void();

    return 0;
  }
  catch (...)
  {
    VtkIgaErrorMacro("Parsing file " << string(file_name_) << ":\n"
                     << "AN UNKNOWN EXCEPTION OCCUR!");

    iga_grid_container_ = iga::paraview_plugin::VtkIgaGridContainer::create_void();

    return 0;
  }
}



void
IgatoolsParaViewReader::
set_grid_type(int arg,
              const char *const name,
              iga::paraview_plugin::VtkGridType &type)
{
  switch (arg)
  {
    case 0:
      if (type != iga::paraview_plugin::VtkGridType::UnstructuredQuadratic)
      {
        type = iga::paraview_plugin::VtkGridType::UnstructuredQuadratic;
        this->Modified();
      }
      break;
    case 1:
      if (type != iga::paraview_plugin::VtkGridType::UnstructuredLinear)
      {
        type = iga::paraview_plugin::VtkGridType::UnstructuredLinear;
        this->Modified();
      }
      break;
    case 2:
      if (type != iga::paraview_plugin::VtkGridType::Structured)
      {
        type = iga::paraview_plugin::VtkGridType::Structured;
        this->Modified();
      }
      break;
    default:
      Assert(arg >= 0 && arg < 3, iga::ExcIndexRange(arg, 0, 3));
      break;
  }
}



void
IgatoolsParaViewReader::
set_num_vis_elements(int arg1, int arg2, int arg3,
                     const char *const name,
                     const char *const mesh_type,
                     NumCells_ &arr)
{
  if ((arr[0] != arg1) || (arr[1] != arg2) || (arr[2] != arg3))
  {
    arr[0] = arg1;
    arr[1] = arg2;
    arr[2] = arg3;
    this->Modified();
  }

  if (arg1 < 1 || arg2 < 1 || arg3 < 1)
  {
    if (arg1 < 2) arr[0] = 1;
    if (arg2 < 2) arr[1] = 1;
    if (arg3 < 2) arr[2] = 1;

    VtkIgaWarningMacro(<< "Invalid specified number of visualization "
                       << "elements per Bezier for the " << mesh_type
                       << " mesh(" << arg1 << ", " << arg2 << ", " << arg3
                       << "). All the values must be >= 1.\nThe number of "
                       << "elements was automatically set to (" << arr[0]
                       << ", " << arr[1] << ", " << arr[2] << ").\n");
  }
}



void
IgatoolsParaViewReader::
SetNumVisualizationElementsPhysicalSolid(int arg1, int arg2, int arg3)
{
  this->set_num_vis_elements(arg1, arg2, arg3,
                             "NumVisualizationElementsPhysicalSolid",
                             "physical solid",
                             n_vis_elem_phys_solid_);
}



void
IgatoolsParaViewReader::
SetNumVisualizationElementsPhysicalKnot(int arg1, int arg2, int arg3)
{
  this->set_num_vis_elements(arg1, arg2, arg3,
                             "NumVisualizationElementsPhysicalKnot",
                             "physical knot",
                             n_vis_elem_phys_knot_);
}



void
IgatoolsParaViewReader::
SetNumVisualizationElementsParametricSolid(int arg1, int arg2, int arg3)
{
  this->set_num_vis_elements(arg1, arg2, arg3,
                             "NumVisualizationElementsParametricSolid",
                             "parametric solid",
                             n_vis_elem_parm_solid_);
}



void
IgatoolsParaViewReader::
SetNumVisualizationElementsParametricKnot(int arg1, int arg2, int arg3)
{
  this->set_num_vis_elements(arg1, arg2, arg3,
                             "NumVisualizationElementsParametricKnot",
                             "parametric knot",
                             n_vis_elem_parm_knot_);
};



void
IgatoolsParaViewReader::
SetGridTypePhysicalSolid(int arg)
{
  this->set_grid_type(arg, "GridTypePhysicalSolid", phys_sol_grid_type_);
}



void
IgatoolsParaViewReader::
SetGridTypePhysicalKnot(int arg)
{
  this->set_grid_type(arg, "GridTypePhysicalKnot", phys_knt_grid_type_);
  Assert(phys_knt_grid_type_ != iga::paraview_plugin::VtkGridType::Structured,
         iga::ExcMessage("Knot mesh must be unstructured."));
}



void
IgatoolsParaViewReader::
SetGridTypePhysicalControl(int arg)
{
  this->set_grid_type(arg, "GridTypePhysicalControl", phys_ctr_grid_type_);
  Assert(phys_ctr_grid_type_ != iga::paraview_plugin::VtkGridType::UnstructuredQuadratic,
         iga::ExcMessage("Control mesh cannot be quadratic."));
}



void
IgatoolsParaViewReader::
SetGridTypeParametricSolid(int arg)
{
  this->set_grid_type(arg, "GridTypeParametricSolid", parm_sol_grid_type_);
}



void
IgatoolsParaViewReader::
SetGridTypeParametricKnot(int arg)
{
  this->set_grid_type(arg, "GridTypeParametricKnot", parm_knt_grid_type_);
  Assert(parm_knt_grid_type_ != iga::paraview_plugin::VtkGridType::Structured,
         iga::ExcMessage("Knot mesh must be unstructured."));
}



void
IgatoolsParaViewReader::
SetSolidMeshPhysical(bool arg)
{
  auto &name  = this->create_sol_mesh_phys_;
  if (name != arg)
  {
    name = arg;
    this->Modified();
  }
}



void
IgatoolsParaViewReader::
SetKnotMeshPhysical(bool arg)
{
  auto &name  = this->create_knt_mesh_phys_;
  if (name != arg)
  {
    name = arg;
    this->Modified();
  }
}



void
IgatoolsParaViewReader::
SetControlMeshPhysical(bool arg)
{
  auto &name  = this->create_ctr_mesh_phys_;
  if (name != arg)
  {
    name = arg;
    this->Modified();
  }
}



void
IgatoolsParaViewReader::
SetSolidMeshParametric(bool arg)
{
  auto &name  = this->create_sol_mesh_parm_;
  if (name != arg)
  {
    name = arg;
    this->Modified();
  }
}



void
IgatoolsParaViewReader::
SetKnotMeshParametric(bool arg)
{
  auto &name  = this->create_knt_mesh_parm_;
  if (name != arg)
  {
    name = arg;
    this->Modified();
  }
}



void
IgatoolsParaViewReader::
SetParametricMesh(bool arg)
{
  auto &name  = this->create_parametric_mesh_;
  if (name != arg)
  {
    name = arg;
    this->Modified();
  }
}



void
IgatoolsParaViewReader::
SetPhysicalMesh(bool arg)
{
  auto &name  = this->create_physical_mesh_;
  if (name != arg)
  {
    name = arg;
    this->Modified();
  }
}



void
IgatoolsParaViewReader::
SetFileName(const char *arg)
{
  if (this->file_name_ && arg && (!strcmp(this->file_name_, arg)))
    return;

  if (this->file_name_)
  {
    delete [] this->file_name_;
  }

  if (arg)
  {
    this->file_name_ = new char[strlen(arg)+1];
    strcpy(this->file_name_, arg);

    parse_input_file_ = true;
  }
  else
  {
    this->file_name_ = NULL;
  }

  this->Modified();
}



int
IgatoolsParaViewReader::
GetNumberOfPhysGeomArrays()
{
  if (iga_grid_container_ == nullptr)
    return 0;
  return iga_grid_container_->get_number_physical_domains();
}



const char *
IgatoolsParaViewReader::
GetPhysGeomArrayName(int index)
{
  Assert(iga_grid_container_ != nullptr, iga::ExcNullPtr());
  const char *name = iga_grid_container_->get_physical_domain_name(index);
  return name;
}



int
IgatoolsParaViewReader::
GetPhysGeomArrayStatus(const char *name)
{
  Assert(iga_grid_container_ != nullptr, iga::ExcNullPtr());
  return iga_grid_container_->get_physical_domain_status(string(name));
}



void
IgatoolsParaViewReader::
SetPhysGeomArrayStatus(const char *name, int enable)
{
  // Note: sometimes this function is called before parsing and
  // names gotten from Previous ParaView session are parsed.
  // The if is introduced for fixing this problem.
  if (iga_grid_container_ != nullptr)
  {
    const auto name_str = string(name);
    if (iga_grid_container_->get_physical_domain_status(name_str) != enable)
    {
      iga_grid_container_->set_physical_domain_status(name_str, enable);
      this->Modified();
    }
  }
}



int
IgatoolsParaViewReader::
GetNumberOfParmGeomArrays()
{
  if (iga_grid_container_ == nullptr)
    return 0;
  return iga_grid_container_->get_number_parametric_domains();
}



const char *
IgatoolsParaViewReader::
GetParmGeomArrayName(int index)
{
  Assert(iga_grid_container_ != nullptr, iga::ExcNullPtr());
  const char *name = iga_grid_container_->get_parametric_domain_name(index);
  return name;
}



int
IgatoolsParaViewReader::
GetParmGeomArrayStatus(const char *name)
{
  Assert(iga_grid_container_ != nullptr, iga::ExcNullPtr());
  return iga_grid_container_->get_parametric_domain_status(string(name));
}



void
IgatoolsParaViewReader::
SetParmGeomArrayStatus(const char *name, int enable)
{
  // Note: sometimes this function is called before parsing and
  // names gotten from Previous ParaView session are parsed.
  // The if is introduced for fixing this problem.
  if (iga_grid_container_ != nullptr)
  {
    const auto name_str = string(name);
    if (iga_grid_container_->get_parametric_domain_status(name_str) != enable)
    {
      iga_grid_container_->set_parametric_domain_status(name_str, enable);
      this->Modified();
    }
  }
}
