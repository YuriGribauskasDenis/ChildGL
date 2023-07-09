
#pragma once

#include <vector>

#include "widget.h"
#include "value.h"
#include "log.h"

/**

\class MultiListBoxWidget

Clase para seleccionar uno o varios elementos de una lista.  Ejemplo de uso:



*/

namespace PGUPV {
  template<typename T = void *>
  class MultiListBoxWidget : public Widget {
  public:
    /**
     Constructor con los elementos de la lista
     \param label Título de la lista
     \param elements Lista de cadenas con los elementos de la lista
     \param selectedElems lista de índices de los elementos seleccionados inicialmente (por defecto, ninguno)
     */
	  MultiListBoxWidget(const std::string &label, const std::vector<std::string> &elements, 
		  const std::vector<unsigned int> &selectedElems = std::vector<unsigned int>{})
      : value(selectedElems), showNumItems(-1) {
      LIBINFO("ListBoxWidget created: " + getName());
      setLabel(label);
      setElements(elements);
	  setSelected(selectedElems, false);
    }

	/**
    \return true si no hay elementos
    */
    bool empty() const {
      return elements.empty();
    }

    /**
    \return la lista de índice de elementos seleccionados
    */
    std::vector<unsigned int> getSelected() const {
      return value.getValue();
    }

	/**
	\return la lista de elementos seleccionados
	*/
	std::vector<std::string> getSelectedElements() const {
		const auto &selection = value.getValue();
		std::vector<std::string> result;
		for (size_t i = 0; i < elements.size(); i++) {
			if (std::find(selection.begin(), selection.end(), i) != selection.end()) {
				result.push_back(elements[i]);
			}
		}
		return result;
	}
    /**
    Establece el elemento seleccionado
    \param idx índice del elemento a seleccionar
    */
    void setSelected(const std::vector<unsigned int> &idxs, bool notifyListeners = true) {
      value.setValue(idxs, notifyListeners);
    }

    /**
    \return una referencia al objeto Value<std::vector<unsigned int> que contiene la selección actual. Con esa
      referencia puedes instalar, por ejemplo, un listener para saber cuándo cambia la selección
    */
    Value<std::vector<unsigned int>>& getValue() { return value; };

    /**
    Establece el número de elementos que serán visibles en la lista
    \param elem El número de elementos que se muestran en la lista sin necesidad de hacer scroll
    */
    void setNumVisibleElements(int elem) { showNumItems = elem; }
    /**
    Modifica los elementos mostrados en la lista.
    \param elements lista de etiquetas a mostrar en el control
    \warning descarta los datos del usuario asociados a la lista, ya que podrían no coincidir
    con los nuevos elementos. Si quieres asociar un dato con cada elemento, vuelve a llamar
    a setUserData.
    */
    void setElements(const std::vector<std::string> &elements, bool notifyListeners = true) {
      this->elements = elements;
      userData.clear();
	  value.resetValue();
	  setSelected(std::vector<unsigned int>{}, notifyListeners);
    }


    /**
    Una listbox puede almacenar tanto la lista de etiquetas (visible para el usuario, que
    las utiliza para seleccionar) como una lista de punteros. Esta segunda lista tiene
    el mismo número de elementos que la primera, pero no es visible para el usuario.
    Se puede obtener el valor asociado a una posición con la llamada ListBoxWidget::getUserData
    \param userData vector de punteros genéricos, del mismo tamaño que el número de elementos de la lista
    */
    void setUserData(std::vector<T> userData) {
      if (userData.size() != elements.size())
        ERRT("El tamaño de los vectores debe ser igual");
      this->userData = userData;
    }

    /**
    \return el valor de usuario asociado a la posición indicada
    */
    T getUserData(unsigned int id) const {
      if (id >= userData.size())
        ERRT("Ese elemento no existe");
      return userData[id];
    }

	void renderWidget() override {
		bool changed = false;
		const auto &selection = value.getValue();
		if (GUILib::MultiListBoxBegin(label)) {
			std::vector<unsigned int> newselection;
			for (auto i = 0; i < elements.size(); i++) {
				bool selected = std::find(selection.begin(), selection.end(), i) != selection.end();
				if (GUILib::MultiListBoxItem(elements[i], &selected)) {
					changed = true;
				}
				if (selected) 
					newselection.push_back(i);
			}
			if (changed)
				value.setValue(newselection);
			GUILib::MultiListBoxEnd();
		}
    }
  protected:
    Value<std::vector<unsigned int>> value;
    std::vector<std::string> elements;
    std::vector<T> userData;
    int showNumItems;
  };
};
