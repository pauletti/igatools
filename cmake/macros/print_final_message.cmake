#-+--------------------------------------------------------------------
# Igatools a general purpose Isogeometric analysis library.
# Copyright (C) 2012-2016  by the igatools authors (see authors.txt).
#
# This file is part of the igatools library.
#
# The igatools library is free software: you can use it, redistribute
# it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, either
# version 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#-+--------------------------------------------------------------------
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# Print important information at the end of cmake configure
#-------------------------------------------------------------------------------
macro(print_final_message)
  message("")
  message("")
  message("******************************************************************")
  message("**")
  message("** [1] To compile and install the library run: ")
  message("** \t make install")
  message("**")
  message("** [2] To use the library it should be made visible to the loader.")
  if(APPLE)
    message("**     Add the following line to your ~/.bash_login")
    message("** \t export DYLD_LIBRARY_PATH=\\")
    message("** \t  $DYLD_LIBRARY_PATH:${CMAKE_INSTALL_PREFIX}/lib")
  else(APPLE)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
      message("**     Add the following line to your ~/.bash_rc")
      message("** \t export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${CMAKE_INSTALL_PREFIX}/lib")
      #    message("** \t  $LD_LIBRARY_PATH:${CMAKE_INSTALL_PREFIX}/lib")   
    endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  endif(APPLE)
  message("**")
  message("** [3] To generate and install the online documentation run: ")
  message("** \t make doc")
  message("**")

  if (IGATOOLS_WITH_PARAVIEW_PLUGIN)
  message("** [4] To use the igatools-ParaView plugin it must be loaded from")
  message("**     ParaView:")
  message("** \t Menu: Tools > Manage Plugins")
  message("**")
  message("**   IMPORTANT NOTE: Please, note that for loading dynamically this")
  message("**                   message plugin ParaView must have be built")
  message("**                   (manually, not just downloading a binary file)")
  message("**                   with shared libraries (i.e. with the option")
  message("**                   BUILD_SHARED_LIBS set to ON).")
  message("**                   For further information visit:")
  message("**                     www.paraview.org/Wiki/ParaView/Plugin_HowTo")
  message("**                     www.paraview.org/Wiki/ParaView:Build_And_Install")
  endif ()

  message("******************************************************************")
  message("")
  message("")
endmacro(print_final_message)
